// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "blake3.hpp"
#include <dci/crypto/blake3.hpp>
#include <dci/utils/dbg.hpp>

namespace
{
    //https://github.com/BLAKE3-team/BLAKE3

    static const uint32_t IV[8] = {0x6A09E667UL, 0xBB67AE85UL, 0x3C6EF372UL,
                                   0xA54FF53AUL, 0x510E527FUL, 0x9B05688CUL,
                                   0x1F83D9ABUL, 0x5BE0CD19UL};

    static const uint8_t MSG_SCHEDULE[7][16] = {
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
        {2, 6, 3, 10, 7, 0, 4, 13, 1, 11, 12, 5, 9, 14, 15, 8},
        {3, 4, 10, 12, 13, 2, 7, 14, 6, 5, 9, 0, 11, 15, 8, 1},
        {10, 7, 12, 9, 14, 3, 13, 15, 4, 0, 11, 2, 5, 8, 1, 6},
        {12, 13, 9, 11, 15, 10, 14, 8, 7, 2, 5, 3, 0, 1, 6, 4},
        {9, 14, 11, 5, 8, 12, 15, 1, 13, 3, 0, 10, 2, 6, 4, 7},
        {11, 15, 5, 0, 1, 9, 8, 6, 14, 10, 2, 12, 3, 4, 7, 13},
    };

#if defined(IS_X86)
#define MAX_SIMD_DEGREE 16
#elif defined(BLAKE3_USE_NEON)
#define MAX_SIMD_DEGREE 4
#else
#define MAX_SIMD_DEGREE 1
#endif

#define MAX_SIMD_DEGREE_OR_2 (MAX_SIMD_DEGREE > 2 ? MAX_SIMD_DEGREE : 2)

    enum blake3_flags {
        CHUNK_START         = 1 << 0,
        CHUNK_END           = 1 << 1,
        PARENT              = 1 << 2,
        ROOT                = 1 << 3,
        KEYED_HASH          = 1 << 4,
        DERIVE_KEY_CONTEXT  = 1 << 5,
        DERIVE_KEY_MATERIAL = 1 << 6,
    };

    typedef struct {
        uint32_t input_cv[8];
        uint64_t counter;
        uint8_t block[BLAKE3_BLOCK_LEN];
        uint8_t block_len;
        uint8_t flags;
    } output_t;


    void blake3_hasher_update(blake3_hasher *self, const void *input,
                              size_t input_len);
    void blake3_hasher_finalize_seek(const blake3_hasher *self, uint64_t seek,
                                     uint8_t *out, size_t out_len);
    void blake3_hasher_finalize(const blake3_hasher *self, uint8_t *out,
                                size_t out_len);

    void blake3_compress_in_place(uint32_t cv[8],
    const uint8_t block[BLAKE3_BLOCK_LEN],
    uint8_t block_len, uint64_t counter,
    uint8_t flags);

    uint32_t load32(const void *src) {
        const uint8_t *p = (const uint8_t *)src;
        return ((uint32_t)(p[0]) << 0) | ((uint32_t)(p[1]) << 8) |
                ((uint32_t)(p[2]) << 16) | ((uint32_t)(p[3]) << 24);
    }

    void store32(void *dst, uint32_t w) {
        uint8_t *p = (uint8_t *)dst;
        p[0] = (uint8_t)(w >> 0);
        p[1] = (uint8_t)(w >> 8);
        p[2] = (uint8_t)(w >> 16);
        p[3] = (uint8_t)(w >> 24);
    }

    // Count the number of 1 bits.
    unsigned int popcnt(uint64_t x) {
#if defined(__GNUC__) || defined(__clang__)
        return __builtin_popcountll(x);
#else
        unsigned int count = 0;
        while (x != 0) {
            count += 1;
            x &= x - 1;
        }
        return count;
#endif
    }

    static unsigned int highest_one(uint64_t x) {
#if defined(__GNUC__) || defined(__clang__)
        return 63 ^ __builtin_clzll(x);
#elif defined(_MSC_VER) && defined(IS_X86_64)
        unsigned long index;
        _BitScanReverse64(&index, x);
        return index;
#elif defined(_MSC_VER) && defined(IS_X86_32)
        if(x >> 32) {
            unsigned long index;
            _BitScanReverse(&index, x >> 32);
            return 32 + index;
        } else {
            unsigned long index;
            _BitScanReverse(&index, x);
            return index;
        }
#else
        unsigned int c = 0;
        if(x & 0xffffffff00000000ULL) { x >>= 32; c += 32; }
        if(x & 0x00000000ffff0000ULL) { x >>= 16; c += 16; }
        if(x & 0x000000000000ff00ULL) { x >>=  8; c +=  8; }
        if(x & 0x00000000000000f0ULL) { x >>=  4; c +=  4; }
        if(x & 0x000000000000000cULL) { x >>=  2; c +=  2; }
        if(x & 0x0000000000000002ULL) {           c +=  1; }
        return c;
#endif
    }

    uint32_t counter_low(uint64_t counter) { return (uint32_t)counter; }

    uint32_t counter_high(uint64_t counter) {
        return (uint32_t)(counter >> 32);
    }

    uint64_t round_down_to_power_of_2(uint64_t x) {
        return 1ULL << highest_one(x | 1);
    }

    void load_key_words(const uint8_t key[BLAKE3_KEY_LEN],
                        uint32_t key_words[8]) {
        key_words[0] = load32(&key[0 * 4]);
        key_words[1] = load32(&key[1 * 4]);
        key_words[2] = load32(&key[2 * 4]);
        key_words[3] = load32(&key[3 * 4]);
        key_words[4] = load32(&key[4 * 4]);
        key_words[5] = load32(&key[5 * 4]);
        key_words[6] = load32(&key[6 * 4]);
        key_words[7] = load32(&key[7 * 4]);
    }

    void store_cv_words(uint8_t bytes_out[32], uint32_t cv_words[8]) {
        store32(&bytes_out[0 * 4], cv_words[0]);
        store32(&bytes_out[1 * 4], cv_words[1]);
        store32(&bytes_out[2 * 4], cv_words[2]);
        store32(&bytes_out[3 * 4], cv_words[3]);
        store32(&bytes_out[4 * 4], cv_words[4]);
        store32(&bytes_out[5 * 4], cv_words[5]);
        store32(&bytes_out[6 * 4], cv_words[6]);
        store32(&bytes_out[7 * 4], cv_words[7]);
    }


    void chunk_state_init(blake3_chunk_state *self, const uint32_t key[8],
    uint8_t flags) {
        memcpy(self->cv, key, BLAKE3_KEY_LEN);
        self->chunk_counter = 0;
        memset(self->buf, 0, BLAKE3_BLOCK_LEN);
        self->buf_len = 0;
        self->blocks_compressed = 0;
        self->flags = flags;
    }

    size_t chunk_state_len(const blake3_chunk_state *self) {
        return (BLAKE3_BLOCK_LEN * (size_t)self->blocks_compressed) +
                ((size_t)self->buf_len);
    }

    size_t chunk_state_fill_buf(blake3_chunk_state *self,
                                const uint8_t *input, size_t input_len) {
        size_t take = BLAKE3_BLOCK_LEN - ((size_t)self->buf_len);
        if (take > input_len) {
            take = input_len;
        }
        uint8_t *dest = self->buf + ((size_t)self->buf_len);
        memcpy(dest, input, take);
        self->buf_len += (uint8_t)take;
        return take;
    }

    uint8_t chunk_state_maybe_start_flag(const blake3_chunk_state *self) {
        if (self->blocks_compressed == 0) {
            return CHUNK_START;
        } else {
            return 0;
        }
    }

    void chunk_state_update(blake3_chunk_state *self, const uint8_t *input,
                            size_t input_len) {
        if (self->buf_len > 0) {
            size_t take = chunk_state_fill_buf(self, input, input_len);
            input += take;
            input_len -= take;
            if (input_len > 0) {
                blake3_compress_in_place(
                            self->cv, self->buf, BLAKE3_BLOCK_LEN, self->chunk_counter,
                            self->flags | chunk_state_maybe_start_flag(self));
                self->blocks_compressed += 1;
                self->buf_len = 0;
                memset(self->buf, 0, BLAKE3_BLOCK_LEN);
            }
        }

        while (input_len > BLAKE3_BLOCK_LEN) {
            blake3_compress_in_place(self->cv, input, BLAKE3_BLOCK_LEN,
                                     self->chunk_counter,
                                     self->flags | chunk_state_maybe_start_flag(self));
            self->blocks_compressed += 1;
            input += BLAKE3_BLOCK_LEN;
            input_len -= BLAKE3_BLOCK_LEN;
        }

        size_t take = chunk_state_fill_buf(self, input, input_len);
        input += take;
        input_len -= take;
    }

    output_t make_output(const uint32_t input_cv[8],
    const uint8_t block[BLAKE3_BLOCK_LEN],
    uint8_t block_len, uint64_t counter,
    uint8_t flags) {
        output_t ret;
        memcpy(ret.input_cv, input_cv, 32);
        memcpy(ret.block, block, BLAKE3_BLOCK_LEN);
        ret.block_len = block_len;
        ret.counter = counter;
        ret.flags = flags;
        return ret;
    }

    output_t chunk_state_output(const blake3_chunk_state *self) {
        uint8_t block_flags =
                self->flags | chunk_state_maybe_start_flag(self) | CHUNK_END;
        return make_output(self->cv, self->buf, self->buf_len, self->chunk_counter,
                           block_flags);
    }

    void chunk_state_reset(blake3_chunk_state *self, const uint32_t key[8],
    uint64_t chunk_counter) {
        memcpy(self->cv, key, BLAKE3_KEY_LEN);
        self->chunk_counter = chunk_counter;
        self->blocks_compressed = 0;
        memset(self->buf, 0, BLAKE3_BLOCK_LEN);
        self->buf_len = 0;
    }


    void hasher_init_base(blake3_hasher *self, const uint32_t key[8],
    uint8_t flags) {
        memcpy(self->key, key, BLAKE3_KEY_LEN);
        chunk_state_init(&self->chunk, key, flags);
        self->cv_stack_len = 0;
    }

    void blake3_hasher_init(blake3_hasher *self) { hasher_init_base(self, IV, 0); }

    void blake3_hasher_init_keyed(blake3_hasher *self,
                                  const uint8_t key[BLAKE3_KEY_LEN]) {
        uint32_t key_words[8];
        load_key_words(key, key_words);
        hasher_init_base(self, key_words, KEYED_HASH);
    }

    void blake3_hasher_init_derive_key_raw(blake3_hasher *self, const void *context,
                                           size_t context_len) {
        blake3_hasher context_hasher;
        hasher_init_base(&context_hasher, IV, DERIVE_KEY_CONTEXT);
        blake3_hasher_update(&context_hasher, context, context_len);
        uint8_t context_key[BLAKE3_KEY_LEN];
        blake3_hasher_finalize(&context_hasher, context_key, BLAKE3_KEY_LEN);
        uint32_t context_key_words[8];
        load_key_words(context_key, context_key_words);
        hasher_init_base(self, context_key_words, DERIVE_KEY_MATERIAL);
    }

    void output_chaining_value(const output_t *self, uint8_t cv[32]) {
        uint32_t cv_words[8];
        memcpy(cv_words, self->input_cv, 32);
        blake3_compress_in_place(cv_words, self->block, self->block_len,
                                 self->counter, self->flags);
        store_cv_words(cv, cv_words);
    }

    output_t parent_output(const uint8_t block[BLAKE3_BLOCK_LEN],
                           const uint32_t key[8], uint8_t flags) {
        return make_output(key, block, BLAKE3_BLOCK_LEN, 0, flags | PARENT);
    }

    void hasher_merge_cv_stack(blake3_hasher *self, uint64_t total_len) {
        size_t post_merge_stack_len = (size_t)popcnt(total_len);
        while (self->cv_stack_len > post_merge_stack_len) {
            uint8_t *parent_node =
                    &self->cv_stack[(self->cv_stack_len - 2) * BLAKE3_OUT_LEN];
            output_t output = parent_output(parent_node, self->key, self->chunk.flags);
            output_chaining_value(&output, parent_node);
            self->cv_stack_len -= 1;
        }
    }

    void hasher_push_cv(blake3_hasher *self, uint8_t new_cv[BLAKE3_OUT_LEN],
                        uint64_t chunk_counter) {
        hasher_merge_cv_stack(self, chunk_counter);
        memcpy(&self->cv_stack[self->cv_stack_len * BLAKE3_OUT_LEN], new_cv,
                BLAKE3_OUT_LEN);
        self->cv_stack_len += 1;
    }

    void hash_one(const uint8_t *input, size_t blocks,
                  const uint32_t key[8], uint64_t counter,
    uint8_t flags, uint8_t flags_start,
    uint8_t flags_end, uint8_t out[BLAKE3_OUT_LEN]) {
        uint32_t cv[8];
        memcpy(cv, key, BLAKE3_KEY_LEN);
        uint8_t block_flags = flags | flags_start;
        while (blocks > 0) {
            if (blocks == 1) {
                block_flags |= flags_end;
            }
            blake3_compress_in_place(cv, input, BLAKE3_BLOCK_LEN, counter,
                                     block_flags);
            input = &input[BLAKE3_BLOCK_LEN];
            blocks -= 1;
            block_flags = flags;
        }
        store_cv_words(out, cv);
    }

    void blake3_hash_many(const uint8_t *const *inputs, size_t num_inputs,
                          size_t blocks, const uint32_t key[8],
    uint64_t counter, bool increment_counter,
    uint8_t flags, uint8_t flags_start,
    uint8_t flags_end, uint8_t *out) {
        while (num_inputs > 0) {
            hash_one(inputs[0], blocks, key, counter, flags, flags_start,
                    flags_end, out);
            if (increment_counter) {
                counter += 1;
            }
            inputs += 1;
            num_inputs -= 1;
            out = &out[BLAKE3_OUT_LEN];
        }
    }

    size_t compress_parents_parallel(const uint8_t *child_chaining_values,
                                     size_t num_chaining_values,
                                     const uint32_t key[8], uint8_t flags,
    uint8_t *out) {
#if defined(BLAKE3_TESTING)
        assert(2 <= num_chaining_values);
        assert(num_chaining_values <= 2 * MAX_SIMD_DEGREE_OR_2);
#endif

        const uint8_t *parents_array[MAX_SIMD_DEGREE_OR_2];
        size_t parents_array_len = 0;
        while (num_chaining_values - (2 * parents_array_len) >= 2) {
            parents_array[parents_array_len] =
                    &child_chaining_values[2 * parents_array_len * BLAKE3_OUT_LEN];
            parents_array_len += 1;
        }

        blake3_hash_many(parents_array, parents_array_len, 1, key,
                         0, // Parents always use counter 0.
                         false, flags | PARENT,
                         0, // Parents have no start flags.
                         0, // Parents have no end flags.
                         out);

        // If there's an odd child left over, it becomes an output.
        if (num_chaining_values > 2 * parents_array_len) {
            memcpy(&out[parents_array_len * BLAKE3_OUT_LEN],
                    &child_chaining_values[2 * parents_array_len * BLAKE3_OUT_LEN],
                    BLAKE3_OUT_LEN);
            return parents_array_len + 1;
        } else {
            return parents_array_len;
        }
    }

    size_t blake3_simd_degree(void) {
        return 1;
    }

    size_t compress_chunks_parallel(const uint8_t *input, size_t input_len,
                                    const uint32_t key[8],
    uint64_t chunk_counter, uint8_t flags,
    uint8_t *out) {
#if defined(BLAKE3_TESTING)
        assert(0 < input_len);
        assert(input_len <= MAX_SIMD_DEGREE * BLAKE3_CHUNK_LEN);
#endif

        const uint8_t *chunks_array[MAX_SIMD_DEGREE];
        size_t input_position = 0;
        size_t chunks_array_len = 0;
        while (input_len - input_position >= BLAKE3_CHUNK_LEN) {
            chunks_array[chunks_array_len] = &input[input_position];
            input_position += BLAKE3_CHUNK_LEN;
            chunks_array_len += 1;
        }

        blake3_hash_many(chunks_array, chunks_array_len,
                         BLAKE3_CHUNK_LEN / BLAKE3_BLOCK_LEN, key, chunk_counter,
                         true, flags, CHUNK_START, CHUNK_END, out);

        // Hash the remaining partial chunk, if there is one. Note that the empty
        // chunk (meaning the empty message) is a different codepath.
        if (input_len > input_position) {
            uint64_t counter = chunk_counter + (uint64_t)chunks_array_len;
            blake3_chunk_state chunk_state;
            chunk_state_init(&chunk_state, key, flags);
            chunk_state.chunk_counter = counter;
            chunk_state_update(&chunk_state, &input[input_position],
                               input_len - input_position);
            output_t output = chunk_state_output(&chunk_state);
            output_chaining_value(&output, &out[chunks_array_len * BLAKE3_OUT_LEN]);
            return chunks_array_len + 1;
        } else {
            return chunks_array_len;
        }
    }

    size_t left_len(size_t content_len) {
        // Subtract 1 to reserve at least one byte for the right side. content_len
        // should always be greater than BLAKE3_CHUNK_LEN.
        size_t full_chunks = (content_len - 1) / BLAKE3_CHUNK_LEN;
        return round_down_to_power_of_2(full_chunks) * BLAKE3_CHUNK_LEN;
    }

    static size_t blake3_compress_subtree_wide(const uint8_t *input,
                                               size_t input_len,
                                               const uint32_t key[8],
    uint64_t chunk_counter,
    uint8_t flags, uint8_t *out) {
        // Note that the single chunk case does *not* bump the SIMD degree up to 2
        // when it is 1. If this implementation adds multi-threading in the future,
        // this gives us the option of multi-threading even the 2-chunk case, which
        // can help performance on smaller platforms.
        if (input_len <= blake3_simd_degree() * BLAKE3_CHUNK_LEN) {
            return compress_chunks_parallel(input, input_len, key, chunk_counter, flags,
                                            out);
        }

        // With more than simd_degree chunks, we need to recurse. Start by dividing
        // the input into left and right subtrees. (Note that this is only optimal
        // as long as the SIMD degree is a power of 2. If we ever get a SIMD degree
        // of 3 or something, we'll need a more complicated strategy.)
        size_t left_input_len = left_len(input_len);
        size_t right_input_len = input_len - left_input_len;
        const uint8_t *right_input = &input[left_input_len];
        uint64_t right_chunk_counter =
                chunk_counter + (uint64_t)(left_input_len / BLAKE3_CHUNK_LEN);

        // Make space for the child outputs. Here we use MAX_SIMD_DEGREE_OR_2 to
        // account for the special case of returning 2 outputs when the SIMD degree
        // is 1.
        uint8_t cv_array[2 * MAX_SIMD_DEGREE_OR_2 * BLAKE3_OUT_LEN];
        size_t degree = blake3_simd_degree();
        if (left_input_len > BLAKE3_CHUNK_LEN && degree == 1) {
            // The special case: We always use a degree of at least two, to make
            // sure there are two outputs. Except, as noted above, at the chunk
            // level, where we allow degree=1. (Note that the 1-chunk-input case is
            // a different codepath.)
            degree = 2;
        }
        uint8_t *right_cvs = &cv_array[degree * BLAKE3_OUT_LEN];

        // Recurse! If this implementation adds multi-threading support in the
        // future, this is where it will go.
        size_t left_n = blake3_compress_subtree_wide(input, left_input_len, key,
                                                     chunk_counter, flags, cv_array);
        size_t right_n = blake3_compress_subtree_wide(
                    right_input, right_input_len, key, right_chunk_counter, flags, right_cvs);

        // The special case again. If simd_degree=1, then we'll have left_n=1 and
        // right_n=1. Rather than compressing them into a single output, return
        // them directly, to make sure we always have at least two outputs.
        if (left_n == 1) {
            memcpy(out, cv_array, 2 * BLAKE3_OUT_LEN);
            return 2;
        }

        // Otherwise, do one layer of parent node compression.
        size_t num_chaining_values = left_n + right_n;
        return compress_parents_parallel(cv_array, num_chaining_values, key, flags,
                                         out);
    }

    void compress_subtree_to_parent_node(
            const uint8_t *input, size_t input_len, const uint32_t key[8],
    uint64_t chunk_counter, uint8_t flags, uint8_t out[2 * BLAKE3_OUT_LEN]) {
#if defined(BLAKE3_TESTING)
        assert(input_len > BLAKE3_CHUNK_LEN);
#endif

        uint8_t cv_array[MAX_SIMD_DEGREE_OR_2 * BLAKE3_OUT_LEN];
        size_t num_cvs = blake3_compress_subtree_wide(input, input_len, key,
                                                      chunk_counter, flags, cv_array);
        dbgAssert(num_cvs <= MAX_SIMD_DEGREE_OR_2);

        // If MAX_SIMD_DEGREE is greater than 2 and there's enough input,
        // compress_subtree_wide() returns more than 2 chaining values. Condense
        // them into 2 by forming parent nodes repeatedly.
        uint8_t out_array[MAX_SIMD_DEGREE_OR_2 * BLAKE3_OUT_LEN / 2];
        // The second half of this loop condition is always true, and we just
        // asserted it above. But GCC can't tell that it's always true, and if NDEBUG
        // is set on platforms where MAX_SIMD_DEGREE_OR_2 == 2, GCC emits spurious
        // warnings here. GCC 8.5 is particular sensitive, so if you're changing this
        // code, test it against that version.
        while (num_cvs > 2 && num_cvs <= MAX_SIMD_DEGREE_OR_2) {
            num_cvs =
                    compress_parents_parallel(cv_array, num_cvs, key, flags, out_array);
            memcpy(cv_array, out_array, num_cvs * BLAKE3_OUT_LEN);
        }
        memcpy(out, cv_array, 2 * BLAKE3_OUT_LEN);
    }

    void blake3_hasher_update(blake3_hasher *self, const void *input,
                              size_t input_len) {
        // Explicitly checking for zero avoids causing UB by passing a null pointer
        // to memcpy. This comes up in practice with things like:
        //   std::vector<uint8_t> v;
        //   blake3_hasher_update(&hasher, v.data(), v.size());
        if (input_len == 0) {
            return;
        }

        const uint8_t *input_bytes = (const uint8_t *)input;

        // If we have some partial chunk bytes in the internal chunk_state, we need
        // to finish that chunk first.
        if (chunk_state_len(&self->chunk) > 0) {
            size_t take = BLAKE3_CHUNK_LEN - chunk_state_len(&self->chunk);
            if (take > input_len) {
                take = input_len;
            }
            chunk_state_update(&self->chunk, input_bytes, take);
            input_bytes += take;
            input_len -= take;
            // If we've filled the current chunk and there's more coming, finalize this
            // chunk and proceed. In this case we know it's not the root.
            if (input_len > 0) {
                output_t output = chunk_state_output(&self->chunk);
                uint8_t chunk_cv[32];
                output_chaining_value(&output, chunk_cv);
                hasher_push_cv(self, chunk_cv, self->chunk.chunk_counter);
                chunk_state_reset(&self->chunk, self->key, self->chunk.chunk_counter + 1);
            } else {
                return;
            }
        }

        // Now the chunk_state is clear, and we have more input. If there's more than
        // a single chunk (so, definitely not the root chunk), hash the largest whole
        // subtree we can, with the full benefits of SIMD (and maybe in the future,
        // multi-threading) parallelism. Two restrictions:
        // - The subtree has to be a power-of-2 number of chunks. Only subtrees along
        //   the right edge can be incomplete, and we don't know where the right edge
        //   is going to be until we get to finalize().
        // - The subtree must evenly divide the total number of chunks up until this
        //   point (if total is not 0). If the current incomplete subtree is only
        //   waiting for 1 more chunk, we can't hash a subtree of 4 chunks. We have
        //   to complete the current subtree first.
        // Because we might need to break up the input to form powers of 2, or to
        // evenly divide what we already have, this part runs in a loop.
        while (input_len > BLAKE3_CHUNK_LEN) {
            size_t subtree_len = round_down_to_power_of_2(input_len);
            uint64_t count_so_far = self->chunk.chunk_counter * BLAKE3_CHUNK_LEN;
            // Shrink the subtree_len until it evenly divides the count so far. We know
            // that subtree_len itself is a power of 2, so we can use a bitmasking
            // trick instead of an actual remainder operation. (Note that if the caller
            // consistently passes power-of-2 inputs of the same size, as is hopefully
            // typical, this loop condition will always fail, and subtree_len will
            // always be the full length of the input.)
            //
            // An aside: We don't have to shrink subtree_len quite this much. For
            // example, if count_so_far is 1, we could pass 2 chunks to
            // compress_subtree_to_parent_node. Since we'll get 2 CVs back, we'll still
            // get the right answer in the end, and we might get to use 2-way SIMD
            // parallelism. The problem with this optimization, is that it gets us
            // stuck always hashing 2 chunks. The total number of chunks will remain
            // odd, and we'll never graduate to higher degrees of parallelism. See
            // https://github.com/BLAKE3-team/BLAKE3/issues/69.
            while ((((uint64_t)(subtree_len - 1)) & count_so_far) != 0) {
                subtree_len /= 2;
            }
            // The shrunken subtree_len might now be 1 chunk long. If so, hash that one
            // chunk by itself. Otherwise, compress the subtree into a pair of CVs.
            uint64_t subtree_chunks = subtree_len / BLAKE3_CHUNK_LEN;
            if (subtree_len <= BLAKE3_CHUNK_LEN) {
                blake3_chunk_state chunk_state;
                chunk_state_init(&chunk_state, self->key, self->chunk.flags);
                chunk_state.chunk_counter = self->chunk.chunk_counter;
                chunk_state_update(&chunk_state, input_bytes, subtree_len);
                output_t output = chunk_state_output(&chunk_state);
                uint8_t cv[BLAKE3_OUT_LEN];
                output_chaining_value(&output, cv);
                hasher_push_cv(self, cv, chunk_state.chunk_counter);
            } else {
                // This is the high-performance happy path, though getting here depends
                // on the caller giving us a long enough input.
                uint8_t cv_pair[2 * BLAKE3_OUT_LEN];
                compress_subtree_to_parent_node(input_bytes, subtree_len, self->key,
                                                self->chunk.chunk_counter,
                                                self->chunk.flags, cv_pair);
                hasher_push_cv(self, cv_pair, self->chunk.chunk_counter);
                hasher_push_cv(self, &cv_pair[BLAKE3_OUT_LEN],
                               self->chunk.chunk_counter + (subtree_chunks / 2));
            }
            self->chunk.chunk_counter += subtree_chunks;
            input_bytes += subtree_len;
            input_len -= subtree_len;
        }

        // If there's any remaining input less than a full chunk, add it to the chunk
        // state. In that case, also do a final merge loop to make sure the subtree
        // stack doesn't contain any unmerged pairs. The remaining input means we
        // know these merges are non-root. This merge loop isn't strictly necessary
        // here, because hasher_push_chunk_cv already does its own merge loop, but it
        // simplifies blake3_hasher_finalize below.
        if (input_len > 0) {
            chunk_state_update(&self->chunk, input_bytes, input_len);
            hasher_merge_cv_stack(self, self->chunk.chunk_counter);
        }
    }

    void blake3_hasher_finalize(const blake3_hasher *self, uint8_t *out,
                                size_t out_len) {
        blake3_hasher_finalize_seek(self, 0, out, out_len);
    }

    uint32_t rotr32(uint32_t w, uint32_t c) {
        return (w >> c) | (w << (32 - c));
    }

    void g(uint32_t *state, size_t a, size_t b, size_t c, size_t d,
           uint32_t x, uint32_t y) {
        state[a] = state[a] + state[b] + x;
        state[d] = rotr32(state[d] ^ state[a], 16);
        state[c] = state[c] + state[d];
        state[b] = rotr32(state[b] ^ state[c], 12);
        state[a] = state[a] + state[b] + y;
        state[d] = rotr32(state[d] ^ state[a], 8);
        state[c] = state[c] + state[d];
        state[b] = rotr32(state[b] ^ state[c], 7);
    }

    void round_fn(uint32_t state[16], const uint32_t *msg, size_t round) {
        // Select the message schedule based on the round.
        const uint8_t *schedule = MSG_SCHEDULE[round];

        // Mix the columns.
        g(state, 0, 4, 8, 12, msg[schedule[0]], msg[schedule[1]]);
        g(state, 1, 5, 9, 13, msg[schedule[2]], msg[schedule[3]]);
        g(state, 2, 6, 10, 14, msg[schedule[4]], msg[schedule[5]]);
        g(state, 3, 7, 11, 15, msg[schedule[6]], msg[schedule[7]]);

        // Mix the rows.
        g(state, 0, 5, 10, 15, msg[schedule[8]], msg[schedule[9]]);
        g(state, 1, 6, 11, 12, msg[schedule[10]], msg[schedule[11]]);
        g(state, 2, 7, 8, 13, msg[schedule[12]], msg[schedule[13]]);
        g(state, 3, 4, 9, 14, msg[schedule[14]], msg[schedule[15]]);
    }

    void compress_pre(uint32_t state[16], const uint32_t cv[8],
    const uint8_t block[BLAKE3_BLOCK_LEN],
    uint8_t block_len, uint64_t counter, uint8_t flags) {
        uint32_t block_words[16];
        block_words[0] = load32(block + 4 * 0);
        block_words[1] = load32(block + 4 * 1);
        block_words[2] = load32(block + 4 * 2);
        block_words[3] = load32(block + 4 * 3);
        block_words[4] = load32(block + 4 * 4);
        block_words[5] = load32(block + 4 * 5);
        block_words[6] = load32(block + 4 * 6);
        block_words[7] = load32(block + 4 * 7);
        block_words[8] = load32(block + 4 * 8);
        block_words[9] = load32(block + 4 * 9);
        block_words[10] = load32(block + 4 * 10);
        block_words[11] = load32(block + 4 * 11);
        block_words[12] = load32(block + 4 * 12);
        block_words[13] = load32(block + 4 * 13);
        block_words[14] = load32(block + 4 * 14);
        block_words[15] = load32(block + 4 * 15);

        state[0] = cv[0];
        state[1] = cv[1];
        state[2] = cv[2];
        state[3] = cv[3];
        state[4] = cv[4];
        state[5] = cv[5];
        state[6] = cv[6];
        state[7] = cv[7];
        state[8] = IV[0];
        state[9] = IV[1];
        state[10] = IV[2];
        state[11] = IV[3];
        state[12] = counter_low(counter);
        state[13] = counter_high(counter);
        state[14] = (uint32_t)block_len;
        state[15] = (uint32_t)flags;

        round_fn(state, &block_words[0], 0);
        round_fn(state, &block_words[0], 1);
        round_fn(state, &block_words[0], 2);
        round_fn(state, &block_words[0], 3);
        round_fn(state, &block_words[0], 4);
        round_fn(state, &block_words[0], 5);
        round_fn(state, &block_words[0], 6);
    }

    void blake3_compress_xof(const uint32_t cv[8],
    const uint8_t block[BLAKE3_BLOCK_LEN],
    uint8_t block_len, uint64_t counter,
    uint8_t flags, uint8_t out[64]) {
        uint32_t state[16];
        compress_pre(state, cv, block, block_len, counter, flags);

        store32(&out[0 * 4], state[0] ^ state[8]);
        store32(&out[1 * 4], state[1] ^ state[9]);
        store32(&out[2 * 4], state[2] ^ state[10]);
        store32(&out[3 * 4], state[3] ^ state[11]);
        store32(&out[4 * 4], state[4] ^ state[12]);
        store32(&out[5 * 4], state[5] ^ state[13]);
        store32(&out[6 * 4], state[6] ^ state[14]);
        store32(&out[7 * 4], state[7] ^ state[15]);
        store32(&out[8 * 4], state[8] ^ cv[0]);
        store32(&out[9 * 4], state[9] ^ cv[1]);
        store32(&out[10 * 4], state[10] ^ cv[2]);
        store32(&out[11 * 4], state[11] ^ cv[3]);
        store32(&out[12 * 4], state[12] ^ cv[4]);
        store32(&out[13 * 4], state[13] ^ cv[5]);
        store32(&out[14 * 4], state[14] ^ cv[6]);
        store32(&out[15 * 4], state[15] ^ cv[7]);
    }
    void output_root_bytes(const output_t *self, uint64_t seek, uint8_t *out,
                           size_t out_len) {
        uint64_t output_block_counter = seek / 64;
        size_t offset_within_block = seek % 64;
        uint8_t wide_buf[64];
        while (out_len > 0) {
            blake3_compress_xof(self->input_cv, self->block, self->block_len,
                                output_block_counter, self->flags | ROOT, wide_buf);
            size_t available_bytes = 64 - offset_within_block;
            size_t memcpy_len;
            if (out_len > available_bytes) {
                memcpy_len = available_bytes;
            } else {
                memcpy_len = out_len;
            }
            memcpy(out, wide_buf + offset_within_block, memcpy_len);
            out += memcpy_len;
            out_len -= memcpy_len;
            output_block_counter += 1;
            offset_within_block = 0;
        }
    }

    void blake3_hasher_finalize_seek(const blake3_hasher *self, uint64_t seek,
                                     uint8_t *out, size_t out_len) {
        // Explicitly checking for zero avoids causing UB by passing a null pointer
        // to memcpy. This comes up in practice with things like:
        //   std::vector<uint8_t> v;
        //   blake3_hasher_finalize(&hasher, v.data(), v.size());
        if (out_len == 0) {
            return;
        }

        // If the subtree stack is empty, then the current chunk is the root.
        if (self->cv_stack_len == 0) {
            output_t output = chunk_state_output(&self->chunk);
            output_root_bytes(&output, seek, out, out_len);
            return;
        }
        // If there are any bytes in the chunk state, finalize that chunk and do a
        // roll-up merge between that chunk hash and every subtree in the stack. In
        // this case, the extra merge loop at the end of blake3_hasher_update
        // guarantees that none of the subtrees in the stack need to be merged with
        // each other first. Otherwise, if there are no bytes in the chunk state,
        // then the top of the stack is a chunk hash, and we start the merge from
        // that.
        output_t output;
        size_t cvs_remaining;
        if (chunk_state_len(&self->chunk) > 0) {
            cvs_remaining = self->cv_stack_len;
            output = chunk_state_output(&self->chunk);
        } else {
            // There are always at least 2 CVs in the stack in this case.
            cvs_remaining = self->cv_stack_len - 2;
            output = parent_output(&self->cv_stack[cvs_remaining * 32], self->key,
                    self->chunk.flags);
        }
        while (cvs_remaining > 0) {
            cvs_remaining -= 1;
            uint8_t parent_block[BLAKE3_BLOCK_LEN];
            memcpy(parent_block, &self->cv_stack[cvs_remaining * 32], 32);
            output_chaining_value(&output, &parent_block[32]);
            output = parent_output(parent_block, self->key, self->chunk.flags);
        }
        output_root_bytes(&output, seek, out, out_len);
    }

    void blake3_compress_in_place(uint32_t cv[8],
    const uint8_t block[BLAKE3_BLOCK_LEN],
    uint8_t block_len, uint64_t counter,
    uint8_t flags) {
        uint32_t state[16];
        compress_pre(state, cv, block, block_len, counter, flags);
        cv[0] = state[0] ^ state[8];
        cv[1] = state[1] ^ state[9];
        cv[2] = state[2] ^ state[10];
        cv[3] = state[3] ^ state[11];
        cv[4] = state[4] ^ state[12];
        cv[5] = state[5] ^ state[13];
        cv[6] = state[6] ^ state[14];
        cv[7] = state[7] ^ state[15];
    }
}

namespace dci::crypto::impl
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake3::Blake3(std::size_t digestSize)
        : Mac{digestSize < 1 ? 1 : digestSize}
    {
        dbgAssert(digestSize >= 1);
        blake3_hasher_init(&_hasher);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake3::Blake3(std::size_t digestSize, std::array<std::uint8_t, 32> key)
        : Mac{digestSize < 1 ? 1 : digestSize}
    {
        dbgAssert(digestSize >= 1);
        static_assert(key.size() == BLAKE3_KEY_LEN);
        blake3_hasher_init_keyed(&_hasher, key.data());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake3::Blake3(std::size_t digestSize, const void* kdfMaterial, std::size_t kdfMaterialSize)
        : Mac{digestSize < 1 ? 1 : digestSize}
    {
        dbgAssert(digestSize >= 1);
        blake3_hasher_init_derive_key_raw(&_hasher, kdfMaterial, kdfMaterialSize);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake3::Blake3(const Blake3& from)
        : Mac{from}
    {
        _hasher = from._hasher;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake3::Blake3(Blake3&& from)
        : Mac{std::move(from)}
    {
        _hasher = from._hasher;
        from.clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake3::~Blake3()
    {
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake3& Blake3::operator=(const Blake3& from)
    {
        static_cast<Mac&>(*this) = from;
        _hasher = from._hasher;
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    Blake3& Blake3::operator=(Blake3&& from)
    {
        static_cast<Mac&>(*this) = std::move(from);
        _hasher = from._hasher;
        from.clear();
        return *this;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    HashPtr Blake3::clone()
    {
        return HashPtr
        {
            new crypto::Blake3{himpl::impl2Face<crypto::Blake3>(*this)},
            [](crypto::Hash*p){delete static_cast<crypto::Blake3*>(p);}
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::size_t Blake3::blockSize()
    {
        return BLAKE3_BLOCK_LEN;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake3::add(const void* vdata, std::size_t len)
    {
        if(!len)
        {
            return;
        }

        blake3_hasher_update(&_hasher, vdata, len);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake3::barrier()
    {
        std::array<uint8_t, 32> digest;

        {
            Blake3 clone = *this;
            clone._digestSize = digest.size();
            clone.finish(digest.data());
        }

        add(digest.data(), digest.size());
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake3::finish(void* digest)
    {
        finish(digest, _digestSize);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake3::finish(void* digest, std::size_t customDigestSize)
    {
        blake3_hasher_finalize(&_hasher, static_cast<uint8_t*>(digest), customDigestSize);
        clear();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake3::clear()
    {
        blake3_hasher_init(&_hasher);
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake3::setKey(const void* key, std::size_t len)
    {
        if(BLAKE3_KEY_LEN == len)
        {
            blake3_hasher_init_keyed(&_hasher, static_cast<const std::uint8_t*>(key));
        }
        else
        {
            blake3_hasher_init_derive_key_raw(&_hasher, key, len);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void Blake3::setKdfMaterial(const void* key, std::size_t len)
    {
        blake3_hasher_init_derive_key_raw(&_hasher, key, len);
    }
}
