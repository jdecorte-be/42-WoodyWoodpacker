/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   woody_generic.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdecorte42 <jdecorte42@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 13:06:21 by mbucci            #+#    #+#             */
/*   Updated: 2023/11/20 16:22:33 by mbucci           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

/* INCLUDES */
#include "../libft/include/libft.h"

/* CONSTANTS */
#define PATCH "woody"
#define DFLT_KEY_LEN 16
#define URANDOM "/dev/urandom"

/* ERRORS */
#define USAGE_ERR "usage: ./woody_woodpacker <path/to/binary> <key>"
#define KEYGEN_ERR "could not generate key"
#define CORRUPT_ERR "file is corrupted"
#define FORMAT_ERR "file format is not supported"

/* UTILS */
int write_error(const char *filename, const char *custmsg);
