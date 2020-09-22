# ESP-IDF & Arduino Template

## Installation
1. Make sure you are using a **Python2**  environment in your terminal. See **Pythono Installation** section for details on how to do this.
2. Install ESP-IDF at the v3.3.2 release (if you have not already) 
	1. Check you current version
 `git describe --always --tags --dirty`
	2. If in doubt, delete your ESP-IDF folder and fresh install  
`git clone -b v3.3.2 --recursive https://github.com/espressif/esp-idf.git`
4. Download this repository
`git clone --recursive https://github.com/oliverjc/idf-arduino-template.git`
5. `make menuconfig`  to configure the project
6. `make -j17` to build


## Python Installation
1. Install [home-brew](https://brew.sh/)
2. Install Anaconda
`brew cask install anaconda`
4. Add Anaconda path to your bash profile. Just insert this line anywhere in your .zshrc or .bashprofile file within your user directory
`export PATH=“/usr/local/anaconda3/bin:$PATH”`
5. Initialise Anaconda for whatever shell you are using. `conda init <SHELL_NAME>` You're probably using either `bash` or `zsh` e.g.
`conda init zsh`
6. Install CCACHE
`brew install cache`
6. Create a Python 2 Environment
`conda create -—name py2 python=2.7`
7. Activate that environment
 `conda activate py2`
8. Add Alias to make activating our environment a lot easier. Add this line to your .zshrc or .bashprofile file. 
`alias pyinit="conda deactivate && conda  activate py2”`
Now whenever we want to activate our python2 environment we simply have to call `pyinit`
To revert to our default environment, just use `conda deactivate`
