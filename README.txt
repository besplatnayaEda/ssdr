#	#	#	#	#	#	#	#	#	#	#	#	#

#	ver 1.0
		- написано по памяти. возможны недочеты.
#
	
#	#	#	#	#	#	#	#	#	#	#	#	#

установка библиотек lcomp и liquid-dsp

если установка производится на свежую машину:

установить через теминал: automake, autoconf
	sudo apt-get install automake
	sudo apt-get install autoconf

установить через терминал: git
	sudo apt-get install git

установить через терминал: fftw
	sudo apt-get install fftw-dev
	
установка библиотеки liquid-dsp

	git clone git://github.com/jgaeddert/liquid-dsp.git
	cd liquid-dsp
	./bootstrap.sh
	./configure
	make 
	sudo make install
	

самое интересное

пропатчить терминал
	в разделе /home/%USERNAME% найти файл .bashrc и добавить в конец:
		export CPATCH=/usr/local/include
		export LIBRARY_PATCH=/usr/local/lib
		export LD_LIBRARY_PATCH=/usr/local/lib
		
		
установка  драйверов и библиотеки lcomp

скачать lcomp
	wget http://www.lcard.ru/download/lcomp_linux.tgz
	tar -xf lcomp_linux.tgz
	
появится в домашнем разделе папка /dstr, в ней дальше работаем

скопировать папку include с заголовочными guiddef.h ifc_ldev.h ioctl.h stubs.h в пакпку с проектом, если там её нет


подправить Makefile в папке lcomp
	для процедуры install (65 стр) добавить :
		cp libcomp.so /usr/local/lib/

выполнить в разделе lcomp
	
	sudo make all
	sudo make install
	
скопировать из корневого каталога  библиотеки lcomp (../dstr/)	файл lcard.rules в папку /etc/udev/rules.d, добавив порядковый номер (например, 80-lcard.rules)
	
	sudo cp xx-lcard.rules /etc/udev/rules.d  
		где xx - порядковый номер, который посмотреть в /etc/udev/rules.d 
	
выполнить make в корневом каталоге библиотеки lcomp
	sudo make modules
	sudo make modules_install

запустить драйвера lcomp
	sudo ./start

соброрка и запуск проекта
	в папке с проектом выполнить make и запустить исполняемый файл
		make
		./%PROJECTNAME%
	
наслаждаться