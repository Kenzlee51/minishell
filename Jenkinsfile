pipeline{
    agent {
        node{
            label 'linux'
        }
    }
    stages{
        stage('Checkout'){
            steps{
                bat 'del /F /S /Q *.*'
                bat 'for /d %% in (.\\*) do @rd /s /q %%x'
                echo 'step Git Checkout'
                checkout scm
            }
        }
        stage('Build'){
            steps{
                echo 'step Build Solution'
                bat make -j12
            }
        }
    }
}
