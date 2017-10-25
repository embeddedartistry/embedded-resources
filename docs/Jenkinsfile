#!groovy
@Library('jenkins-pipeline-lib') _

pipeline
{
  agent any
  environment
  {
    GIT_CHANGE_LOG = gitChangeLog(currentBuild.changeSets)
  }
  triggers
  {
    //At 04:00 on every day-of-week from Monday through Friday.
    pollSCM('H 4 * * 1-5')
  }
  stages
  {
    stage('Setup')
    {
      steps
      {
        echo 'Running make clean...'
        sh 'make clean'
      }
    }
    stage('Build')
    {
      steps
      {
        sh 'make'
      }
    }
  }
  post
  {
    always
    {
      //Scan console log for Clang warnings
      warnings(canResolveRelativePaths: true, categoriesPattern: '', consoleParsers: [[parserName: 'Clang (LLVM based)']], defaultEncoding: '', excludePattern: '', healthy: '', includePattern: '', messagesPattern: '', unHealthy: '', useStableBuildAsReference: true)

      // Scan workspace to check open tasks
      openTasks(defaultEncoding: '', excludePattern: 'lib/external/**, os/**, build/gen/**, build/examples/**', healthy: '', high: 'FIXME, FIX', ignoreCase: true, low: '', normal: 'TODO', pattern: '**/*.c, **/*.h, **/*.cpp, **/*.hpp, *.lua **/*.sh', unHealthy: '')

      // Report Status
      slackNotify(currentBuild.currentResult)
    }
    failure
    {
      /*
      * This job does not have a GitHub configuration,
      * so we need to create a dummy config
      */
      githubSetConfig('4c01f168-ca25-483e-bc6d-8d105fc5fb70')
      githubFileIssue()
      emailNotify(currentBuild.currentResult)
    }
  }
}
