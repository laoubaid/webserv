/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Uvec.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laoubaid <laoubaid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/21 22:41:10 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/08/10 18:49:53 by laoubaid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Uvec.hpp"

// protect in case allocation on heap fails

Uvec::Uvec(void)
{
	_vec = new std::vector<unsigned char>;
}

Uvec::Uvec(iterator begin, iterator end)
{
	_vec = new std::vector<unsigned char>(begin, end);
}

Uvec::Uvec(const_iterator begin, const_iterator end)
{
	_vec = new std::vector<unsigned char>(begin, end);
}

Uvec::Uvec(const unsigned char *str, std::size_t len)
{
	_vec = new std::vector<unsigned char>(str, str + len);
}

Uvec::Uvec(unsigned char start, unsigned char end)
{
	_vec = new std::vector<unsigned char>;
	for (int i = start; i <= end; ++i)
        (*_vec).push_back(static_cast<unsigned char>(i));
}

Uvec::Uvec(const Uvec &other)
{
	if (this != &other)
		_vec = new std::vector<unsigned char>(other._vec->begin(), other._vec->end());
}

Uvec&	Uvec::operator=(const Uvec &other)
{
	if (this != &other)
	{
		delete _vec;
		_vec = new std::vector<unsigned char>(other._vec->begin(), other._vec->end());
	}
	return (*this);
}

Uvec::~Uvec(void)
{
	delete _vec;
}

Uvec::iterator	Uvec::begin(void)
{
	return (_vec->begin());
}

Uvec::const_iterator	Uvec::begin(void) const
{
	return (_vec->begin());
}

Uvec::iterator	Uvec::end(void)
{
	return (_vec->end());
}

Uvec::const_iterator	Uvec::end(void) const
{
	return (_vec->end());
}

Uvec&	Uvec::operator+(const Uvec &other)
{
	// std::cout << "plus op called\n";
	_vec->insert(_vec->end(), other._vec->begin(), other._vec->end());
	return (*this);
}

Uvec&	Uvec::operator+=(const Uvec &other)
{
	// std::cout << "plus eq op called\n";
	_vec->insert(_vec->end(), other._vec->begin(), other._vec->end());
	return (*this);
}

unsigned char&	Uvec::operator[](std::size_t index)
{
	if (index >= _vec->size())
		throw std::out_of_range("index given is out of range");
	return ((*_vec)[index]);
}

bool    Uvec::operator==(const Uvec& other) const
{
	if (_vec == NULL && other._vec == NULL) {
            return true;
        }
        if (_vec == NULL || other._vec == NULL) {
            return false;
        }
	return (*_vec == *other._vec);
}

bool    Uvec::operator!=(const Uvec& other) const
{
	return (*_vec != *other._vec);
}

void	Uvec::push_back(unsigned char c)
{
	_vec->push_back(c);
}

std::size_t		Uvec::size(void) const
{
	return (_vec->size());
}

bool	Uvec::has(unsigned char c) const
{
	if (this->find(c) == this->end())
		return (false);
	return (true);
}

Uvec::iterator	Uvec::find(unsigned char c)
{
	return (std::find(_vec->begin(), _vec->end(), c));
}

Uvec::const_iterator	Uvec::find(unsigned char c) const
{
	return (std::find(_vec->begin(), _vec->end(), c));
}

Uvec::iterator	Uvec::find(const Uvec &other)
{
	return (std::search(_vec->begin(), _vec->end(), other.begin(), other.end()));
}

Uvec::const_iterator	Uvec::find(const Uvec &other) const
{
	return (std::search(_vec->begin(), _vec->end(), other.begin(), other.end()));
}

Uvec::iterator	Uvec::find(iterator begin, iterator end, const Uvec &other)
{
	return (std::search(begin, end, other.begin(), other.end()));
}

Uvec::const_iterator	Uvec::find(const_iterator begin, const_iterator end, const Uvec &other) const
{
	return (std::search(begin, end, other.begin(), other.end()));
}

// to be deleted
void	Uvec::print(void) const
{
	std::cout << "[ ";
	for (std::vector<unsigned char>::iterator it = _vec->begin(); it != _vec->end(); it++)
	{
		if (std::isprint(*it))
			std::cout << *it;
		else
			std::cout << "0x" << std::hex << (int)*it;
		if (it +1 != _vec->end())
			std::cout << ", ";
	}
	std::cout << " ]\n";
	std::cout << std::dec; // reset to decimal base
}
