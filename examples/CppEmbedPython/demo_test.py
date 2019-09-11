# This Python file uses the following encoding: utf-8

#coding:utf-8
import os
import requests

def hello(name):
   return 'hello '+ name

def request():
    response = requests.get("http://www.baidu.com/")
    return response.text

if __name__=='__main__':
   #print(hello('python'))
   print(request())
