%clc;
%clear all;
%
%filename = 'test.dat';
%delimiter = '';
%formatSpec = '%f%[^\n\r]';
%fileID = fopen(filename,'r');
%dataArray = textscan(fileID, formatSpec, 'Delimiter', delimiter, 'EmptyValue' ,NaN, 'ReturnOnError', false);
%fclose(fileID);
dat = load('test.m');
y1=dat(:,1);
y2=dat(:,2);
y=dat(:,3);
y0=dat(:,4);
x = dat(:,5);
t = 1:length(y1);
figure(1); 
plot(t,y1,t,y2);
figure(2);
plot(t,y,t,y0);
figure(3);
plot(t,x);

Fs = 10000;
L = (Fs*2);
f = (Fs/2*linspace(0,1,L/2));
y = abs(fft(x,L)/L);
figure(4)
semilogy(f(1:200),y(1:200))