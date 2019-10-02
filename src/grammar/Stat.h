/**********************************************
    > File Name: Stat.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Sat Sep 28 10:40:51 2019
 **********************************************/

#ifndef STAT_H
#define STAT_H

class Stat {
	class Cond {
		static void cond(void);
	public:
		static void _if(void);
		static void _while(void);
		static void _do(void);
		static void _for(void);
	};
	static void read(void);
	static void write(void);
	static void ret(void);
	static void assign(void);
	static void stat(void);
public:
	static void block(void);
};

#endif /* STAT_H */
