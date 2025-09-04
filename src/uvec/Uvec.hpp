/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Uvec.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kez-zoub <kez-zoub@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/21 22:40:11 by kez-zoub          #+#    #+#             */
/*   Updated: 2025/09/04 10:29:57 by kez-zoub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UVEC_HPP
# define UVEC_HPP

# include <vector>
# include <stdexcept>
# include <algorithm>
# include <iostream>

class Uvec
{
	private:
		std::vector<unsigned char>	*_vec;
	public:
		typedef std::vector<unsigned char>::iterator	iterator;
		typedef std::vector<unsigned char>::const_iterator	const_iterator;
		
		Uvec(void);
		Uvec(iterator begin, iterator end);
		Uvec(const_iterator begin, const_iterator end);
		Uvec(const unsigned char *str, std::size_t len);
		Uvec(unsigned char start, unsigned char end);
		Uvec(const Uvec &other);
		Uvec&	operator=(const Uvec &other);
		~Uvec(void);

		iterator	begin(void);
		const_iterator	begin(void) const;
		iterator	end(void);
		const_iterator	end(void) const;
		Uvec&	operator+(const Uvec &other);
		Uvec&	operator+=(const Uvec &other);
		unsigned char&	operator[](std::size_t index);
		unsigned char&	operator[](std::size_t index) const;

		bool    operator==(const Uvec& other) const;
		bool    operator!=(const Uvec& other) const;

		void		push_back(unsigned char c);
		void		clear(void);
		std::size_t	size(void) const;
		bool		has(unsigned char c) const;
		iterator	find(unsigned char c);
		const_iterator	find(unsigned char c) const;
		iterator	find(const Uvec &other);
		const_iterator	find(const Uvec &other) const;
		iterator	find(iterator begin, iterator end, const Uvec &other);
		const_iterator	find(const_iterator begin, const_iterator end, const Uvec &other) const;
};
std::ostream&	operator<<(std::ostream& out, const Uvec& vec);

#endif
