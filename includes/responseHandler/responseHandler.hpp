/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   responseHandler.hpp                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: tmullan <tmullan@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/03/30 18:12:40 by tmullan       #+#    #+#                 */
/*   Updated: 2022/04/01 16:08:06 by tmullan       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "requestHandler.hpp"

class	responseHandler {

	public:
		responseHandler(requestHandler&	request);
		~responseHandler();
	private:
		responseHandler(/* Maybe take a pointer to the requesthandler */);
		requestHandler	*_request;


};
