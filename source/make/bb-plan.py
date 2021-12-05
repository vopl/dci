# -*- python -*-
# ex: set filetype=python:

from buildbot.plugins import *
import time

############################################################################################################
bbfileSteps=[]

#######
#bbfileSteps.append(steps.RemoveDirectory(
#    name = 'clean build directory',
#    dir = "build",))

bbfileSteps.append(steps.SetPropertiesFromEnv(
    name = 'load bb properties',
    variables = ["NPROC", "AUPSDIR"]))

#######
bbfileSteps.append(steps.CMake(
    name = 'configure',
    workdir = 'build',
    path = '../src',
    generator = 'Ninja',
    definitions  = {
        'CMAKE_BUILD_TYPE' : 'Release',
        'DCI_SRC_BRANCH'   : util.Interpolate("%(prop:branch:-)s"),
        'DCI_SRC_REVISION' : util.Interpolate("%(prop:revision:-)s"),
        'DCI_PROVIDER'     : 'bb',
        'DCI_AUP_SIGNERKEY': util.Interpolate("%(secret:signerKey:-)s"),
    },
    haltOnFailure = True,
    warnOnWarnings = True,))

bbfileSteps.append(steps.Compile(
    name = 'compile',
    workdir = 'build',
    command = ['ninja', 'all', '-j', util.Interpolate("%(prop:NPROC:-1)s")],
    warningPattern = "^(.*?):([0-9]+).*?[Ww]arning: (.*)$",
    warningExtractor = steps.Compile.warnExtractFromRegexpGroups,
    haltOnFailure = True,
    warnOnWarnings = True,))

#######
@util.renderer
def dciHostCmd(props):
    if 'windows'==props.getProperty('buildername'):
        return 'dci-host.cmd'
    return './dci-host'

#######
def appendTests(workdir, label):
    for stage in ['noenv', 'mnone', 'mstart']:
        bbfileSteps.append(steps.Test(
            name = f'test {stage} ({label})',
            workdir = workdir,
            command = [util.Interpolate('%(kw:exe)s', exe=dciHostCmd), '--test', stage],
            warningPattern = "^....-..-.. ..:..:...... (WRN|ERR|FTL): ",
            env = {'QT_QPA_PLATFORM': 'minimal'},
            haltOnFailure = True,
            warnOnWarnings = True,))

#######
appendTests('build/out/bin', 'after build')

#######
bbfileSteps.append(steps.Compile(
    name = 'make aup image',
    workdir = 'build',
    command = ['ninja', 'aup-image', '-j', util.Interpolate("%(prop:NPROC:-1)s")],
    haltOnFailure = True,
    warnOnWarnings = True,))

#######
bbfileSteps.append(steps.ShellCommand(
    name = 'apply aup',
    workdir = 'build/out/bin',
    command = [util.Interpolate('%(kw:exe)s', exe=dciHostCmd), '--aup', 'targetDir=../../out2', 'stateDir=../var/aup', 'target.fileKind=*'],))

#######
appendTests('build/out2/bin', 'after aup')

#######
@util.renderer
def aupDir(props):
    return (time.strftime("%Y-%m-%d_%H-%M-%S") + "/" + props.getProperty('buildername') + "/" + props.getProperty('branch')).replace("/", "_")

bbfileSteps.append(steps.ShellCommand(
    name = 'move out aup',
    workdir = 'build/out/var',
    command = ['mv', "-v", "--no-target-directory", "aup", util.Interpolate('%(prop:AUPSDIR:-aups)s/%(kw:aupDir)s', aupDir=aupDir)],))

#bbfileSteps.append(steps.RemoveDirectory(
#    name = 'clean build directory again',
#    dir = "build",))
