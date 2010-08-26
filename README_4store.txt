4store SPARQL endpoints for testing:
http://dbtune.org/classical/sparql/
http://cima.ng-london.org.uk/raphael/sparql/


Install 4store on debian squeeze (testing)
# Dependencies
sudo apt-get install build-essential libpcre3-dev librasqal2-dev libraptor1-dev libglib2.0-dev ncurses-dev libreadline-dev
# 4store
sudo apt-get install git-core
git clone http://github.com/garlik/4store.git
cd 4store
make
make install
make test

sudo apt-get install curl

