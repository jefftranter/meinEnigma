# git pull git@github.com:lpaseen/meinEnigma.git master

default: upload

upload: myEnigma.ino
	${HOME}/arduino/arduino --upload --board arduino:avr:nano myEnigma.ino

verify: myEnigma.ino
	${HOME}/arduino/arduino --verify --board arduino:avr:nano myEnigma.ino
