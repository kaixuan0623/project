# Image Filtering
Description:
In this project, I build a simple pipeline for manipulating bitmap image files by applying a set of common filters on the images, which will allow the user to do things like turn the image to greyscale, blur the image, and increase the image size.

Each image filter will be a separate program that reads in a bitmap image from its standard input, calculates some transformation on the image's pixels and possibly its dimensions, and then writes a transformed bitmap image to its standard output.

In order to provide a convenient interface to the user to run several filters on a single image, I create a "master" program that spawns a separate process for each filter the user specifies. This program will also perform some basic process management, waiting for all of its child process to complete, and reporting any errors that occur.

This program is intended for practicing and gaining hands-on experiences on the concept of process, processes creation and inter-process communication using pipe and also involving manipulation of binary files. 
