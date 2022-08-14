cmake_minimum_required(VERSION 3.0)

cmake_policy(SET CMP0076 NEW)

target_sources(jtoc PUBLIC
        ft_atoi.cpp
        ft_bzero.cpp
        ft_isdigit.cpp
        ft_itoa.cpp
        ft_memalloc.cpp
        ft_memcpy.cpp
        ft_strcpy.cpp
        ft_strdup.cpp
        ft_strhash.cpp
        ft_strlen.cpp
        ft_strncmp.cpp
        ft_strnew.cpp
        ft_strsub.cpp
        jtoc_get.cpp
        jtoc_get_raw_data.cpp
        jtoc_node.cpp
        jtoc_parse.cpp
        jtoc_parse_number.cpp
        jtoc_parse_value.cpp
        jtoc_read.cpp
        jtoc_read_file.cpp
        jtoc_utils.cpp
        jtoc_utils2.cpp
        jtoc_validate.cpp
        jtoc_validate_value.cpp
        jtoc_write.cpp
        )