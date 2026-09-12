#  ,---.          |         |    ,---.         |          ,---.|    
#  |---',---.,---.|--- ,---.|    |    ,---.,---|,---.,---.|---'|    
#  |    |   |`---.|    ,---||    |    |   ||   ||---'`---.|    |    
#  `    `---'`---'`---'`---^`---'`---'`---'`---'`---'`---'`    `---'
#
# Copyright (c) 2016-2026 Michal Babik
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Information about postal codes based on data from the website
# http://www.kody-pocztowe.biz
# Informacje o kodach pocztowych na postawie danych
# ze strony http://www.kody-pocztowe.biz
#-----------------------------------------------------------------------------#
#                      SQLite tables used in database                         #
#-----------------------------------------------------------------------------#
#                                                                             #
# TABLE      post_codes                                                       #
#                                                                             #
#            id             INTEGER PRIMARY KEY      identifier               #
#            code1          TEXT                     first code number        #
#            code2          TEXT                     second code number       #
#            city_id        INTEGER                  city identifier          #
#            city_det_id    INTEGER                  city detail identifier   #
#            street_id      INTEGER                  street identifier        #
#            street_no_id   INTEGER                  street number identifier #
#            post_un        TEXT                     post unit                #
#            voivod_id      INTEGER                  voivodeship identifier   #
#                                                                             #
#-----------------------------------------------------------------------------#
#                                                                             #
# TABLE      city                                                             #
#                                                                             #
#            id             INTEGER PRIMARY KEY      identifier               #
#            city_name      TEXT                     name of the city         #
#                                                                             #
#-----------------------------------------------------------------------------#
#                                                                             #
# TABLE      city_det                                                         #
#                                                                             #
#            id             INTEGER PRIMARY KEY      identifier               #
#            city_detcr     TEXT                     description (district)   #
#                                                                             #
#-----------------------------------------------------------------------------#
#                                                                             #
# TABLE      street                                                           #
#                                                                             #
#            id             INTEGER PRIMARY KEY      identifier               #
#            street_name    TEXT                     name of the street       #
#                                                                             #
#-----------------------------------------------------------------------------#
#                                                                             #
# TABLE      street_no                                                        #
#                                                                             #
#            id             INTEGER PRIMARY KEY      identifier               #
#            street_number  TEXT                     street numbers           #
#                                                                             #
#-----------------------------------------------------------------------------#
#                                                                             #
# TABLE      voivod                                                           #
#                                                                             #
#            id             INTEGER PRIMARY KEY      identifier               #
#            voivodeship    TEXT                     voivodeship name         #
#                                                                             #
#-----------------------------------------------------------------------------#
import sqlite3

DATABASE_FILE_NAME = "pc_base.db"
#-----------------------------------------------------------------------------#
def db_open(db_file_name: str | None = None):
    """Open database file"""
    db_file_name = db_file_name or DATABASE_FILE_NAME
    con = sqlite3.connect(db_file_name)
    con.text_factory = str
    return con
#-----------------------------------------------------------------------------#
def sql_command_get(cmd, args=(), bn=None):
    return sql_command_exec(cmd, args, True, bn, False)
#-----------------------------------------------------------------------------#
def sql_command_save(cmd, args=(), bn=None):
    return sql_command_exec(cmd, args, False, bn, True)
#-----------------------------------------------------------------------------#
def sql_command_exec(cmd, args=(), return_result=False, bn=None, comm=True
                     ) -> tuple[bool, list | str]:
    """Execute sqlite command"""
    return_status = False
    return_value: list | str = []
    #ret = [False, None]
    c = None
    con = None
    try:
        con = db_open(bn)
        c = con.cursor()
        c.execute(cmd, args)
        if return_result:
            return_value = c.fetchall()
            #ret[1] = c.fetchall()
        if comm:
            con.commit()
    except sqlite3.Error as e:
        print("An error occurred:", e)
        if con:
            con.rollback()
        return_value = str(e)
        #ret[1] = str(e)
    else:
        #ret[0] = True
        return_status = True
    finally:
        if c:
            c.close()
        if con:
            con.close()
    return return_status, return_value
    #return ret
#-----------------------------------------------------------------------------#
def sql_get_post_code_info(code1: str, code2: str):
    r, dt = sql_command_get(
            """select city_name, city_detcr, street_name, street_number, 
            post_un, voivodeship from post_codes inner join city on 
            post_codes.city_id = city.id inner join street on 
            post_codes.street_id = street.id inner join street_no on 
            post_codes.street_no_id = street_no.id inner join city_det on 
            post_codes.city_det_id = city_det.id inner join voivod on 
            post_codes.voivod_id = voivod.id where code1=? and code2=?
            order by city.city_name""",
            (code1, code2,))
    return r, dt
#-----------------------------------------------------------------------------#
def sql_get_city_voivodeship(city_id: int):
    r, dt = sql_command_get(
            """select voivod.id, voivodeship from voivod inner join post_codes
            on voivod.id = post_codes.voivod_id where city_id = ? order by
            voivodeship""", (city_id,))
    return r, dt
#-----------------------------------------------------------------------------#
def sql_get_info(city_id: int, voivodeship_id: int, street_name: str):
    r, dt = sql_command_get(
            """select code1, code2, city_name, city_detcr, street_name,
            street_number, post_un, voivodeship from post_codes inner join city
            on post_codes.city_id = city.id inner join street on 
            post_codes.street_id = street.id inner join street_no on 
            post_codes.street_no_id = street_no.id inner join city_det on 
            post_codes.city_det_id = city_det.id inner join voivod on 
            post_codes.voivod_id = voivod.id where city.id=? and voivod.id=?
            and street.street_name like ? order by code1, code2""",
            (city_id, voivodeship_id, '%' + street_name + '%',))
    return r, dt
#-----------------------------------------------------------------------------#
def sql_get_city_names_like(city_name: str):
    r, dt = sql_command_get(
            """select * from city where city_name like ? order by city_name""",
            ('%' + city_name + '%',))
    return r, dt
#-----------------------------------------------------------------------------#

