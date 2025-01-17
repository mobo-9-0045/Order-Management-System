# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: aomman <marvin@42.fr>                      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/24 12:40:40 by aomman            #+#    #+#              #
#    Updated: 2023/04/24 12:40:43 by aomman           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRC = main.cpp src/OrderService.cpp

NAME = btc

all : ${NAME}

${NAME} : ${SRC}
	c++ -Wall -Wextra -Werror ${SRC} -o ${NAME} -L/usr/local/lib -I/usr/local/include -lcpr -lpthread

clean : 
	rm -f ${NAME}

fclean : clean

re : fclean all