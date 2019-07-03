#pragma once
#ifndef _STDAFX_H
#define _CRT_SECURE_NO_WARNINGS

#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) // 设置入口地址
#include<windows.h>

//#include<GL\glew.h>
#include<GL\GL.h>
#include<GL\GLU.h>
#include<GL\glut.h>

#include<cstdio>
#include<iostream>
#include<fstream>
#include<sstream>
#include<iomanip>

#include<string>
#include<vector>
#include<memory>

#include<algorithm>
#include<queue>
#include<list>
#include<deque>
#include<map>
#include<set>
#include<numeric>
#include<functional>

#include<cmath>
#include<cstdlib>

#include<stdexcept>
#include<ctime>

using namespace std;

template<typename  T>
string str(T t)
{
	ostringstream ss;
	ss << t;
	return ss.str();
}

class MyClass//带父指针的class
{
private:
	void* father{ nullptr };
public:
	MyClass() {}
	MyClass(void* father) :father{ father } {}
	void SetFather(void* father_new) { father = father_new; }
};
#endif // !_STDAFX_H
