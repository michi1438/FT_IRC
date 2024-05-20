/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: robin <robin@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/06 10:46:01 by robin             #+#    #+#             */
/*   Updated: 2024/03/06 11:29:30 by robin            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RPN_HPP
# define RPN_HPP

#include <iostream>
#include <stack>

class RPN{
private:
        std::stack<double> numbers;
    
public:
        RPN(void) {};
        RPN(std::stack<double> number);
        RPN(const RPN & src);
        RPN & operator=(const RPN & rhs);
        ~RPN() {};

        void doOp(std::string & input);
};

#endif
