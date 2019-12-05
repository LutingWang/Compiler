/**********************************************
    > File Name: StrPool.h
    > Author: Luting Wang
    > Mail: 2457348692@qq.com 
    > Created Time: Wed Nov 13 19:56:19 2019
 **********************************************/

#ifndef STR_POOL_H
#define STR_POOL_H

#include <map>
#include <string>

class StrPool {
	std::map<std::string, std::string> _pool;

	int _size(void) const;

	void _insert(const std::string&);
public:
	void init(void);

	const std::string& operator [] (const std::string&) const;

	void output(void) const;
};

extern StrPool strpool;

#endif /* STR_POOL_H */
