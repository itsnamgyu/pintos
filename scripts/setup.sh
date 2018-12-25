# usage ./setup.sh

echo "source `ls -d $PWD/init.sh`" >> ~/.bash_profile

# vim tags
cd ~/pintos/src
ctags -R *

# reset disk
~/pintos/scripts/reset-disk

source ~/.bash_profile
