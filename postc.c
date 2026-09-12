/** 
 *  ,---.          |         |    ,---.         |          ,---.|    
 *  |---',---.,---.|--- ,---.|    |    ,---.,---|,---.,---.|---'|    
 *  |    |   |`---.|    ,---||    |    |   ||   ||---'`---.|    |    
 *  `    `---'`---'`---'`---^`---'`---'`---'`---'`---'`---'`    `---'
 *
 * @file  postc.c
 * @copyright Copyright (C) 2019-2026 Michal Babik
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @brief Postal code database example use
 *
 * Example usr of postal codes sqlite database in c.
 *
 * @date July 23, 2026
 * @version 1.2.0
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sqlite3.h> 
#include <string.h>
#include "utf8.h"
/*----------------------------------------------------------------------------*/
/* gcc -std=c11 -Wall postc.c utf8.c -lsqlite3 -o postc                       */
/*----------------------------------------------------------------------------*/
/**
 * @brief  Postal code info structure
 */
typedef struct Postal_data {
    char code1[3];              //!< First part of postal code
    char code2[4];              //!< Second part of postal code
    char city_name[35];         //!< Name of the city
    char voivodeship[25];       //!< Voivodeship name
    char city_detcr[35];        //!< City detail description
    char street_name[100];      //!< Name of the street
    char street_number[40];     //!< Street number
    char post_un[60];           //!< Post unit
} Postal_data;
/*----------------------------------------------------------------------------*/
/**
 * @brief  City info structure
 */
typedef struct City_data {
    int  i_id;              //!< City id in database
    int  i_no;              //!< City number in list
    char city_name[35];     //!< Name of the city
    int  voivod_id;         //!< Voivodeship id in database
    char voivodeship[25];   //!< Voivodeship name
} City_data;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Clear Postal_data structure
 *
 * @param[in] pd    Postal_data to clear
 * @return    None
 */
static void
clear_pdata (Postal_data *pd)
{
    memset (pd->code1, 0, sizeof (pd->code1));
    memset (pd->code2, 0, sizeof (pd->code2));
    memset (pd->city_name, 0, sizeof (pd->city_name));
    memset (pd->voivodeship, 0, sizeof (pd->voivodeship));
    memset (pd->city_detcr, 0, sizeof (pd->city_detcr));
    memset (pd->street_name, 0, sizeof (pd->street_name));
    memset (pd->street_number, 0, sizeof (pd->street_number));
    memset (pd->post_un, 0, sizeof (pd->post_un));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Clear City_data structure
 *
 * @param[in] c_data    City data to clear
 * @return    None
 */
static void
clear_cdata (City_data *c_data)
{
    c_data->i_id = 0;
    c_data->i_no = 0;
    c_data->voivod_id = 0;
    memset (c_data->city_name, 0, sizeof (c_data->city_name));
    memset (c_data->voivodeship, 0, sizeof (c_data->voivodeship));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get all postal code info of a given postal code.
 *
 * Connects to postal code database, gets all cities, stret names, etc of a
 * given postal code and puts in Postal_data items. Items are later appended to
 * a pd_data poiter.
 *
 * @param[out] pd_data  Pointer to Postal_data items to save info
 * @param[in]  code1    Firts part of postal code
 * @param[in]  code2    Second part of postal code
 * @return     Number of items saved to pd_data
 */
static uint16_t
get_post_code_info (Postal_data **pd_data,
                    char         *code1,
                    char         *code2)
{
    sqlite3 *db;
    uint16_t i_rcnt = 0;
    int rc; 
    const char *sql_c = 
        "SELECT city_name, city_detcr, street_name, street_number, post_un, \
        voivodeship FROM post_codes INNER JOIN city ON \
        post_codes.city_id = city.id INNER JOIN street ON post_codes.street_id \
        = street.id INNER JOIN street_no ON post_codes.street_no_id = \
        street_no.id INNER JOIN city_det ON post_codes.city_det_id = \
        city_det.id INNER JOIN voivod ON post_codes.voivod_id = voivod.id \
        WHERE code1=? AND code2=? ORDER BY city.city_name";
    rc = sqlite3_open ("pc_base.db", &db);
    if (rc) {
        fprintf (stderr, "Can't open database: %s\n", sqlite3_errmsg (db));
        sqlite3_close (db);
        return 0;
    }
    sqlite3_stmt *res;
    rc = sqlite3_prepare_v2 (db, sql_c, -1, &res, NULL);
    if (rc != SQLITE_OK) {
        fprintf (stderr, "Can't prepare query: %s\n", sqlite3_errmsg (db));
        sqlite3_close (db);
        return 0;
    }

    sqlite3_bind_text (res, 1, code1, -1, SQLITE_STATIC);
    sqlite3_bind_text (res, 2, code2, -1, SQLITE_STATIC);

    rc = sqlite3_step (res);
    while (rc == SQLITE_ROW) {
        i_rcnt++;
        if (*pd_data == NULL) {
            *pd_data = malloc (sizeof (Postal_data));
            clear_pdata (*pd_data);
        }
        else {
            *pd_data = realloc (*pd_data, i_rcnt * sizeof (Postal_data));
            clear_pdata (& (*pd_data)[i_rcnt-1]);
        }
        snprintf ( (*pd_data)[i_rcnt-1].code1, sizeof ((*pd_data)[i_rcnt-1].code1), "%s", code1);
        snprintf ( (*pd_data)[i_rcnt-1].code2, sizeof ((*pd_data)[i_rcnt-1].code2), "%s", code2);
        snprintf ( (*pd_data)[i_rcnt-1].city_name, sizeof ((*pd_data)[i_rcnt-1].city_name), "%s",
                sqlite3_column_text (res, 0));
        snprintf ( (*pd_data)[i_rcnt-1].city_detcr, sizeof ((*pd_data)[i_rcnt-1].city_detcr), "%s",
                sqlite3_column_text (res, 1));
        snprintf ( (*pd_data)[i_rcnt-1].street_name, sizeof ((*pd_data)[i_rcnt-1].street_name), "%s",
                sqlite3_column_text (res, 2));
        snprintf ( (*pd_data)[i_rcnt-1].street_number, sizeof ((*pd_data)[i_rcnt-1].street_number), "%s",
                sqlite3_column_text (res, 3));
        snprintf ( (*pd_data)[i_rcnt-1].post_un, sizeof ((*pd_data)[i_rcnt-1].post_un), "%s",
                sqlite3_column_text (res, 4));
        snprintf ( (*pd_data)[i_rcnt-1].voivodeship, sizeof ((*pd_data)[i_rcnt-1].voivodeship), "%s",
                sqlite3_column_text (res, 5));
        rc = sqlite3_step (res);
        }
    if (rc != SQLITE_DONE) {
        fprintf (stderr, "Query failed: %s\n", sqlite3_errmsg (db));
    }
	sqlite3_finalize (res);
    sqlite3_close (db);
    return i_rcnt;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get all city info similar to a given ch_city string.
 *
 * Connects to postal code database, finds all cities with names similar to
 * a given ch_city string and puts them in a City_data items and appended to a
 * c_data pointer.
 *
 * @param[out] c_data  Pointer to City_data items to save info
 * @param[in]  ch_city Name of a city
 * @return     Number of items saved to c_data
 */
static uint16_t
get_city_names_like (City_data **c_data,
                     const char *ch_city)
{
    sqlite3 *db;
    uint16_t i_rcnt = 0;
    int rc; 
    const char *sql_c = 
        "SELECT DISTINCT city.id, city.city_name, voivod.id, voivod.voivodeship\
         FROM city INNER JOIN post_codes ON city.id = post_codes.city_id INNER \
         JOIN voivod ON post_codes.voivod_id = voivod.id where city.city_name \
         like ? order by city.city_name";
    rc = sqlite3_open ("pc_base.db", &db);
    if (rc) {
        fprintf (stderr, "Can't open database: %s\n", sqlite3_errmsg (db));
        sqlite3_close (db);
        return 0;
    }
    sqlite3_stmt *res;
    rc = sqlite3_prepare_v2 (db, sql_c, -1, &res, NULL);
    if (rc != SQLITE_OK) {
        fprintf (stderr, "Can't prepare query: %s\n", sqlite3_errmsg (db));
        sqlite3_close (db);
        return 0;
    }

    sqlite3_bind_text (res, 1, ch_city, -1, SQLITE_STATIC);

    rc = sqlite3_step (res);
    while (rc == SQLITE_ROW) {
        i_rcnt++;
        if (*c_data == NULL) {
            *c_data = malloc (sizeof (City_data));
            clear_cdata (*c_data);
        }
        else {
            *c_data = realloc (*c_data, i_rcnt * sizeof (City_data));
            clear_cdata (& (*c_data)[i_rcnt-1]);
        }
        (*c_data)[i_rcnt-1].i_no = i_rcnt;
        (*c_data)[i_rcnt-1].i_id = sqlite3_column_int (res, 0);
        snprintf ( (*c_data)[i_rcnt-1].city_name, sizeof ((*c_data)[i_rcnt-1].city_name), "%s",
                sqlite3_column_text (res, 1));
        (*c_data)[i_rcnt-1].voivod_id = sqlite3_column_int (res, 2);
        snprintf ( (*c_data)[i_rcnt-1].voivodeship, sizeof ((*c_data)[i_rcnt-1].voivodeship), "%s",
                sqlite3_column_text (res, 3));
        rc = sqlite3_step (res);
        }
    if (rc != SQLITE_DONE) {
        fprintf (stderr, "Query failed: %s\n", sqlite3_errmsg (db));
    }
	sqlite3_finalize (res);
    sqlite3_close (db);
    return i_rcnt;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get all postal code info of a city with i_cid id and voivodeship
 *         with i_vid id.
 *
 * Connects to postal code database, gets all postal code info of a city with
 * i_cid database id and i_vid voivodeship database id. All data is later put
 * in Postal_data items and appended to pd_data pointer.
 *
 * @param[out] pd_data Pointer to Postal_data items to save info
 * @param[in]  i_cid   City database id
 * @param[in]  i_vid   Voivodeship database id
 * @return     Number of items saved to pd_data
 */
static uint16_t
get_city_voivodeship_info (Postal_data **pd_data,
                           int           i_cid,
                           int           i_vid)
{
    sqlite3 *db;
    uint16_t i_rcnt = 0;
    int rc; 
    const char *sql_c =
        "select code1, code2, city_name, city_detcr, street_name, \
        street_number, post_un, voivodeship from post_codes inner join city \
        on post_codes.city_id = city.id inner join street on \
        post_codes.street_id = street.id inner join street_no on  \
        post_codes.street_no_id = street_no.id inner join city_det on \
        post_codes.city_det_id = city_det.id inner join voivod on \
        post_codes.voivod_id = voivod.id where city.id=? and voivod.id=? \
        order by code1, code2";
    rc = sqlite3_open ("pc_base.db", &db);
    if (rc) {
        fprintf (stderr, "Can't open database: %s\n", sqlite3_errmsg (db));
        sqlite3_close (db);
        return 0;
    }
    sqlite3_stmt *res;
    rc = sqlite3_prepare_v2 (db, sql_c, -1, &res, NULL);
    if (rc != SQLITE_OK) {
        fprintf (stderr, "Can't prepare query: %s\n", sqlite3_errmsg (db));
        sqlite3_close (db);
        return 0;
    }

    sqlite3_bind_int (res, 1, i_cid);
    sqlite3_bind_int (res, 2, i_vid);

    rc = sqlite3_step (res);
    while (rc == SQLITE_ROW) {
        i_rcnt++;
        if (*pd_data == NULL) {
            *pd_data = malloc (sizeof (Postal_data));
            clear_pdata (*pd_data);
        }
        else {
            *pd_data = realloc (*pd_data, i_rcnt * sizeof (Postal_data));
            clear_pdata (& (*pd_data)[i_rcnt-1]);
        }
        snprintf ( (*pd_data)[i_rcnt-1].code1, sizeof ((*pd_data)[i_rcnt-1].code1), "%s",
                sqlite3_column_text (res, 0));
        snprintf ( (*pd_data)[i_rcnt-1].code2, sizeof ((*pd_data)[i_rcnt-1].code2), "%s",
                sqlite3_column_text (res, 1));
        snprintf ( (*pd_data)[i_rcnt-1].city_name, sizeof ((*pd_data)[i_rcnt-1].city_name), "%s",
                sqlite3_column_text (res, 2));
        snprintf ( (*pd_data)[i_rcnt-1].city_detcr, sizeof ((*pd_data)[i_rcnt-1].city_detcr), "%s",
                sqlite3_column_text (res, 3));
        snprintf ( (*pd_data)[i_rcnt-1].street_name, sizeof ((*pd_data)[i_rcnt-1].street_name), "%s",
                sqlite3_column_text (res, 4));
        snprintf ( (*pd_data)[i_rcnt-1].street_number, sizeof ((*pd_data)[i_rcnt-1].street_number), "%s",
                sqlite3_column_text (res, 5));
        snprintf ( (*pd_data)[i_rcnt-1].post_un, sizeof ((*pd_data)[i_rcnt-1].post_un), "%s",
                sqlite3_column_text (res, 6));
        snprintf ( (*pd_data)[i_rcnt-1].voivodeship, sizeof ((*pd_data)[i_rcnt-1].voivodeship), "%s",
                sqlite3_column_text (res, 7));
        rc = sqlite3_step (res);
        }
    if (rc != SQLITE_DONE) {
        fprintf (stderr, "Query failed: %s\n", sqlite3_errmsg (db));
    }
	sqlite3_finalize (res);
    sqlite3_close (db);
    return i_rcnt;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Counts max char count of Postal_data strings.
 *
 * @param[in]  pd_data  Pointer to Postal_data items
 * @param[out] i_width  Pointer where to save char count data
 * @param[in]  i_cnt    Number of items in pd_data pointer
 * @return     None
 */
static void
get_pd_column_width (Postal_data *pd_data,
                     uint16_t    *i_width,
                     int          i_cnt)
{
    for (int i = 0; i < i_cnt; ++i) {
        if (u8_strlen (pd_data[i].city_name) > i_width[0])
            i_width[0] = u8_strlen (pd_data[i].city_name);
        if (u8_strlen (pd_data[i].voivodeship) > i_width[1])
            i_width[1] = u8_strlen (pd_data[i].voivodeship);
        if (u8_strlen (pd_data[i].city_detcr) > i_width[2])
            i_width[2] = u8_strlen (pd_data[i].city_detcr);
        if (u8_strlen (pd_data[i].street_name) > i_width[3])
            i_width[3] = u8_strlen (pd_data[i].street_name);
        if (u8_strlen (pd_data[i].street_number) > i_width[4])
            i_width[4] = u8_strlen (pd_data[i].street_number);
        if (u8_strlen (pd_data[i].post_un) > i_width[5])
            i_width[5] = u8_strlen (pd_data[i].post_un);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Prints info of City_data items from c_data pointer.
 *
 * @param[in]  c_data   Pointer to City_data items
 * @param[in]  i_cnt    Number of items in c_data pointer
 * @return     None
 */
static void
print_city_data (City_data *c_data,
                 int        i_cnt)
{
    printf ("%3s | %-35s\n", "No", "Place name");
    for (int i = 0; i < i_cnt; ++i) {
        printf ("%3d | %s (%s)\n",
                c_data[i].i_no,
                c_data[i].city_name,
                c_data[i].voivodeship);
    }
    printf ("\n");
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Prints info of Postal_data items from pd_data pointer.
 *
 * @param[in]  pd_data  Pointer to City_data items
 * @param[in]  i_cnt    Number of items in pd_data pointer
 * @return     None
 */
static void
print_all_info (Postal_data *pd_data,
                int          i_cnt)
{
    uint16_t i_w[6] = {0,0,0,0,0,0};
    get_pd_column_width (pd_data, i_w, i_cnt);
    printf ("| Code  ");
    printf (" | Place");
    for (uint16_t j = 0; j < i_w[0] + i_w[1] - 1; ++j)
        printf (" ");
    if (i_w[2] > 0) {
        printf (" | Place det.");
        for (uint16_t j = 0; j < i_w[2] - 10; ++j)
            printf (" ");
    }
    printf (" | Street");
    for (uint16_t j = 0; j < i_w[3] - 6; ++j)
        printf (" ");
    printf (" | Street no");
    for (uint16_t j = 0; j < i_w[4] - 9; ++j)
        printf (" ");
    if (i_w[5] > 0) {
        printf (" | Post unit");
    }
    printf ("\n");
    for (int i = 0; i < i_cnt; ++i) {
        printf ("| %2s-%3s ", pd_data[i].code1, pd_data[i].code2);
        printf ("| %s", pd_data[i].city_name);
        for (uint16_t j = 0; j < i_w[0] - u8_strlen (pd_data[i].city_name); ++j)
            printf (" ");
        printf (" (%s)", pd_data[i].voivodeship);
        for (uint16_t j = 0; j < i_w[1] - u8_strlen (pd_data[i].voivodeship); ++j)
            printf (" ");
        if (i_w[2] > 0) {
            printf (" | %s", pd_data[i].city_detcr);
            for (uint16_t j = 0; j < i_w[2] - u8_strlen (pd_data[i].city_detcr); ++j)
                printf (" ");
        }
        printf (" | %s", pd_data[i].street_name);
        for (uint16_t j = 0; j < i_w[3] - u8_strlen (pd_data[i].street_name); ++j)
            printf (" ");
        printf (" | %s", pd_data[i].street_number);
        for (uint16_t j = 0; j < i_w[4] - u8_strlen (pd_data[i].street_number); ++j)
            printf (" ");
        if (u8_strlen (pd_data[i].post_un) > 0) {
            printf (" | %s", pd_data[i].post_un);
        }
        printf ("\n");
    }
    printf ("\n");
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Put postal code info from single string to separate ones.
 *
 * @param[out]  code1  First postal code data
 * @param[out]  code2  Second postal code data
 * @param[in]   codes  Postal code in one string
 * @return      None
 */
void set_codes (char *code1, 
                char *code2,
                char *codes)
{
    code1[0] = codes[0];
    code1[1] = codes[1];
    code1[2] = '\0';
    code2[0] = codes[3];
    code2[1] = codes[4];
    code2[2] = codes[5];
    code2[3] = '\0';
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get and print all postal code info based on entered postal code.
 *
 * Prompts for a postal code number, gathers all of that postal code info
 * from database and prints that info on screen.
 *
 * @return      None
 */
static void
get_show_info_by_postal_code (void)
{
    char ch_getcode[7];
    char ch_code1[3];
    char ch_code2[4];
    uint16_t i_rcnt = 0;
    Postal_data *pd_data = NULL;

    memset (ch_getcode, 0, sizeof (ch_getcode));
    memset (ch_code1, 0, sizeof (ch_code1));
    memset (ch_code2, 0, sizeof (ch_code2));

    printf ("Enter postal code : ");
    scanf ("%6s", ch_getcode);
    printf ("\n");
    if (strlen (ch_getcode) == 6) {
        set_codes (ch_code1, ch_code2, ch_getcode);
        printf ("Results for code %s-%s :\n\n", ch_code1, ch_code2);
        i_rcnt = get_post_code_info (&pd_data, ch_code1, ch_code2);
        if (i_rcnt == 0)
            printf ("No results\n");
        print_all_info (pd_data, i_rcnt);
        printf ("\n");
        free (pd_data);
    }
    else {
        printf ("Wrong postal code length\n\n");
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get and print all postal code info based on entered city name.
 *
 * Prompts for a city name, looks in database for such cities and if there
 * are more then one prompts to select the one you want to be showed info.
 *
 * @return      None
 */
static void
get_show_info_by_city_street (void)
{
    Postal_data *pd_data = NULL;
    City_data *c_data = NULL;
    uint16_t i_rcnt = 0;
    int i_sres = 0;
    int i_city = 0;
    char ch_city_t[35];
    char ch_city[37];

    memset (ch_city_t, 0, sizeof (ch_city_t));
    memset (ch_city, 0, sizeof (ch_city));
    printf ("Enter place name   : ");
    i_sres = scanf ("%34s", ch_city_t);
    printf ("\n");
    if (u8_strlen (ch_city_t) < 3) {
        printf ("Place name to short. It shlould have at least 3 letters \n\n");
        return;
    }
    if (strlen (ch_city_t) > 32) {
        printf ("Place name to long. It shlould have at most 32 letters \n\n");
        return;
    }
    printf ("Results for place like %s :\n\n", ch_city_t);
    sprintf (ch_city, "%%%s%%", ch_city_t);
    i_rcnt = get_city_names_like (&c_data ,ch_city);
    if (i_rcnt == 0) {
        printf ("No results\n");
        return;
    }
    print_city_data (c_data, i_rcnt);
    if (i_rcnt > 1) {
        printf ("Choose the place [1-%hu]: ", i_rcnt);
        i_sres = scanf ("%d", &i_city);
        printf ("\n");
        if (i_sres == 0 || i_city > i_rcnt || i_city == 0) {
            printf ("Wrong place number\n\n");
            free (c_data);
            return;
        }
    }
    else if (i_rcnt == 1) {
        i_city = 1;
    }
    int i_vid = 0;
    int i_cid = 0;
    for (int i = 0; i < i_rcnt; ++i) {
        if (c_data[i].i_no == i_city) {
            i_vid = c_data[i].voivod_id;
            i_cid = c_data[i].i_id;
            break;
        }
    }
    i_rcnt = get_city_voivodeship_info (&pd_data, i_cid, i_vid);
    print_all_info (pd_data, i_rcnt);
    free (pd_data);
    free (c_data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief   Main function.
 *
 * @return  Return value
 */
int
main (void)
{
    int i_sres = 0;
    int i_mode = 0;
    printf ("\nDo you want to : \n\n");
    printf ("[1] Enter postal code and show its places and streets \n");
    printf ("[2] Enter place name and find its postal code \n");
    printf ("\nSelect mode [1-2] : ");
    i_sres = scanf ("%d", &i_mode);
    if (i_sres != 0 && i_mode > 0 && i_mode < 3) {
        if (i_mode == 1) {
            get_show_info_by_postal_code ();
        }
        if (i_mode == 2) {
            get_show_info_by_city_street ();
        }
    }
    else {
        printf ("Wrong answer !\n");
    }
    return 0;
}
/*----------------------------------------------------------------------------*/

