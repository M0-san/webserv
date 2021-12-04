/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_response.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbani <mbani@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/07 13:00:35 by mbani             #+#    #+#             */
/*   Updated: 2021/11/23 09:05:22 by mbani            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request_response.hpp"

request_response::request_response():max_fd(-1)
{
	FD_ZERO(&(this->read_fd));
	FD_ZERO(&(this->write_fd));
	FD_ZERO(&(this->tmp_read));
	FD_ZERO(&(this->tmp_write));
}

void		request_response::reset(int fd)
{
	(req_fd[fd]).resetRequest();
	std::cout << fd << " has been reseted " << std::endl;
}

bool		request_response::receive(int fd, Server &server) // return false if connection is closed 
{
	long long status;
	char buffer[BUFFER_SIZE];

	bzero(buffer, BUFFER_SIZE);
	status = recv(fd, buffer, BUFFER_SIZE, 0);
	if (status == 0 || status == -1) // Closed connection or Invalid fd
	{
		remove_fd(fd, true, true); // remove from read set
		server.socketFree(fd);
		return false;
	}
	req_fd[fd].append(buffer, status);
	return true;
}

void	request_response::send_all(int fd, std::string res)
{
	size_t total = 0;
	size_t left = res.length();
	int sent = 0;

	// while(total < res.length())
	// {
		sent = send(fd, res.c_str(), left, 0);
		if (sent == 0) // connection closed
		{
			// remove_fd(fd, true, true); // remove from read set
		}
		// if (sent < 0)
		// 	break;
		// total += sent;
		// left -= sent;
	// }
}

bool	request_response::req_completed(int fd)
{
	return (req_fd[fd]).is_completed();
}

const std::stringstream&		request_response::get_req(int fd)
{
	return this->req_fd[fd].get_req();
}

int 	request_response::is_ready(int fd, bool to_read)
{
	if (to_read)
		return FD_ISSET(fd, &(this->tmp_read));
	else
		return FD_ISSET(fd, &(this->tmp_write));
}

void	request_response::update_set()
{
	this->tmp_read = this->read_fd;
	this->tmp_write = this->write_fd;
}

void request_response::set_fd(int fd, bool to_read, bool is_client) // set fd to read or write sets (1 for read 0 for write) && update max_fd
{
	if (to_read)
	{
		FD_SET(fd, &read_fd);
		if (is_client)
			req_fd.insert(std::make_pair(fd, Request()));
	}
	else 
		FD_SET(fd, &write_fd);
	if (fd > max_fd) // update max_fd
			max_fd = fd;
}

void request_response::remove_fd(int fd, bool to_read, bool is_client, bool _close)
{
	if (to_read)
	{
		FD_CLR(fd, &read_fd);
		if (is_client && _close)
		{
			req_fd.erase(fd);
		}
	}
	else
		FD_CLR(fd, &write_fd);
	
}

bool request_response::select_fd()
{
	if (select(max_fd + 1, &(this->tmp_read), &(this->tmp_write), NULL, NULL) < 0)
	{
		std::cout << "Select Failed !" << std::endl;
		return false;
	}
	return true;
}

int request_response::get_maxfd()const
{
	return this->max_fd;
}

void request_response::close_connection(int fd) const
{
	shutdown(fd, 2); // Further sends and receives are disallowed
	close(fd);
}

std::map<int , Request>& request_response::getMap()
{
	return this->req_fd;
}

request_response::~request_response()
{}