/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Err_page_switch.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mguerga <mguerga@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/30 11:06:25 by mguerga           #+#    #+#             */
/*   Updated: 2024/04/30 11:32:41 by mguerga          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Centralinclude.hpp"

std::string read_errpage(int err_code)
{
	switch (err_code)
	{
		case 403:
		{
			std::ifstream file(ERR_403);
			std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			std::string response = "HTTP/1.1 403 OK\r\nContent-Type: text/html\r\n\r\n" + content;
			return response;
		}
		case 404:
		{
			std::ifstream file(ERR_404);
			std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			std::string response = "HTTP/1.1 404 OK\r\nContent-Type: text/html\r\n\r\n" + content;
			return response;
		}
		case 405:
		{
			std::ifstream file(ERR_405);
			std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			std::string response = "HTTP/1.1 405 OK\r\nContent-Type: text/html\r\n\r\n" + content;
			return response;
		}
		case 413:
		{
			std::ifstream file(ERR_413);
			std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			std::string response = "HTTP/1.1 413 OK\r\nContent-Type: text/html\r\n\r\n" + content;
			return response;
		}
		case 414:
		{
			std::ifstream file(ERR_414);
			std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			std::string response = "HTTP/1.1 414 OK\r\nContent-Type: text/html\r\n\r\n" + content;
			return response;
		}
		case 505:
		{
			std::ifstream file(ERR_505);
			std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			std::string response = "HTTP/1.1 505 OK\r\nContent-Type: text/html\r\n\r\n" + content;
			return response;
		}
		case 512:
		{
			std::ifstream file(ERR_512);
			std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			std::string response = "HTTP/1.1 512 OK\r\nContent-Type: text/html\r\n\r\n" + content;
			return response;
		}
		default:
		{
			std::ifstream file(ERR_500);
			std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			std::string response = "HTTP/1.1 400 OK\r\nContent-Type: text/html\r\n\r\n" + content;
			return response;
		}
	}
}
