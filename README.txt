#	#	#	#	#	#	#	#	#	#	#	#	#

#	ver 1.0
		- �������� �� ������. �������� ��������.
#
	
#	#	#	#	#	#	#	#	#	#	#	#	#

��������� ��������� lcomp � liquid-dsp

���� ��������� ������������ �� ������ ������:

���������� ����� �������: automake, autoconf
	sudo apt-get install automake
	sudo apt-get install autoconf

���������� ����� ��������: git
	sudo apt-get install git

���������� ����� ��������: fftw
	sudo apt-get install fftw-dev
	
��������� ���������� liquid-dsp

	git clone git://github.com/jgaeddert/liquid-dsp.git
	cd liquid-dsp
	./bootstrap.sh
	./configure
	make 
	sudo make install
	

����� ����������

���������� ��������
	� ������� /home/%USERNAME% ����� ���� .bashrc � �������� � �����:
		export CPATCH=/usr/local/include
		export LIBRARY_PATCH=/usr/local/lib
		export LD_LIBRARY_PATCH=/usr/local/lib
		
		
���������  ��������� � ���������� lcomp

������� lcomp
	wget http://www.lcard.ru/download/lcomp_linux.tgz
	tar -xf lcomp_linux.tgz
	
�������� � �������� ������� ����� /dstr, � ��� ������ ��������

����������� ����� include � ������������� guiddef.h ifc_ldev.h ioctl.h stubs.h � ������ � ��������, ���� ��� � ���


���������� Makefile � ����� lcomp
	��� ��������� install (65 ���) �������� :
		cp libcomp.so /usr/local/lib/

��������� � ������� lcomp
	
	sudo make all
	sudo make install
	
����������� �� ��������� ��������  ���������� lcomp (../dstr/)	���� lcard.rules � ����� /etc/udev/rules.d, ������� ���������� ����� (��������, 80-lcard.rules)
	
	sudo cp xx-lcard.rules /etc/udev/rules.d  
		��� xx - ���������� �����, ������� ���������� � /etc/udev/rules.d 
	
��������� make � �������� �������� ���������� lcomp
	sudo make modules
	sudo make modules_install

��������� �������� lcomp
	sudo ./start

�������� � ������ �������
	� ����� � �������� ��������� make � ��������� ����������� ����
		make
		./%PROJECTNAME%
	
������������