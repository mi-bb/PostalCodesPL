# -*- coding: UTF-8 -*-
#
#  ,---.          |         |    ,---.         |          ,---.|    
#  |---',---.,---.|--- ,---.|    |    ,---.,---|,---.,---.|---'|    
#  |    |   |`---.|    ,---||    |    |   ||   ||---'`---.|    |    
#  `    `---'`---'`---'`---^`---'`---'`---'`---'`---'`---'`    `---'
#
#    File:     pcodespl.py
#    Version:  1.1
#    Date:     February 24, 2019
#    Author:   Michal Babik <michalb1981@o2.pl>
#    Copyright (C) 2016-2019 Michal Babik
#
#    Information about postal codes based on data from the website
#    http://www.kody-pocztowe.biz
#    Informacje o kodach pocztowych na postawie danych
#    ze strony http://www.kody-pocztowe.biz
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
#-----------------------------------------------------------------------------#
def b_dbop(bn=None):
    """Open database file"""
    if not bn: bn = 'pc_base.db'
    con = sqlite3.connect(bn)
    con.text_factory = str
    c = con.cursor()
    return con
#-----------------------------------------------------------------------------#
def sql_command_get(cmd, args=(), bn=None):
    return sql_command_exec(cmd, args, True, bn, False)
#-----------------------------------------------------------------------------#
def sql_command_save(cmd, args=(), bn=None):
    return sql_command_exec(cmd, args, False, bn, True)
#-----------------------------------------------------------------------------#
def sql_command_exec(cmd, args=(), rett=False, bn=None, comm=True):
    """Execute sqlite command"""
    ret = [False, None]
    c = None
    con = None
    try:
        con = b_dbop(bn)
        c = con.cursor()
        c.execute(cmd, args)
        if rett: ret[1] = c.fetchall()
        if comm: con.commit()
    except sqlite3.Error as e:
        print("An error occurred:", e)
        con.rollback()
        ret[1] = e
    else:
        ret[0] = True
    finally:
        if c: c.close()
        if con: con.close()
    return ret
#-----------------------------------------------------------------------------#
def sql_get_post_code_info(code1, code2):
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
def sql_get_city_voivodeship(c_id):
    r, dt = sql_command_get(
            """select voivod.id, voivodeship from voivod inner join post_codes
            on voivod.id = post_codes.voivod_id where city_id = ? order by
            voivodeship""", (c_id,))
    return r, dt
#-----------------------------------------------------------------------------#
def sql_get_info(c_id, v_id, street_txt):
    r, dt = sql_command_get(
            """select code1, code2, city_name, city_detcr, street_name,
            street_number, post_un, voivodeship from post_codes inner join city
            on post_codes.city_id = city.id inner join street on 
            post_codes.street_id = street.id inner join street_no on 
            post_codes.street_no_id = street_no.id inner join city_det on 
            post_codes.city_det_id = city_det.id inner join voivod on 
            post_codes.voivod_id = voivod.id where city.id=? and voivod.id=?
            and street.street_name like ? order by code1, code2""",
            (c_id, v_id, '%' + street_txt + '%',))
    return r, dt
#-----------------------------------------------------------------------------#
def sql_get_city_names_like(c_name):
    r, dt = sql_command_get(
            """select * from city where city_name like ? order by city_name""",
            ('%' + c_name + '%',))
    return r, dt
#-----------------------------------------------------------------------------#

