/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  msts.h
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Include file specifying variables needed for the 7 MSTS routes
 *
 *==============================================================================
 */

 #if defined ROUTE_USA1 || defined ROUTE_USA2 || defined ROUTE_JAPAN1 || defined ROUTE_JAPAN2 || defined ROUTE_EUROPE1 || defined ROUTE_EUROPE2 || defined ROUTE_TUTORIAL

#define ROUTE_MSTS


#if defined ROUTE_USA1

char msts_route[] = "USA1" ;
char msts_pdb_file[] = "usa1.trk" ;
char msts_tdb_file[] = "necorid.tdb" ;
char msts_rdb_file[] = "NECorid.rdb" ;

#elif defined ROUTE_USA2

char msts_route[] = "USA2" ;
char msts_pdb_file[] = "usa2.trk" ;
char msts_tdb_file[] = "marias.tdb" ;
char msts_rdb_file[] = "Marias.rdb" ;

#elif defined ROUTE_JAPAN1

char msts_route[] = "JAPAN1" ;
char msts_pdb_file[] = "japan1.trk" ;
char msts_tdb_file[] = "odakyuse.tdb" ;
char msts_rdb_file[] = "OdakyuSE.rdb" ;
#elif defined ROUTE_JAPAN2

char msts_route[] = "JAPAN2" ;
char msts_pdb_file[] = "japan2.trk" ;
char msts_tdb_file[] = "hisatsu.tdb" ;
char msts_rdb_file[] = "Hisatsu.rdb" ;

#elif defined ROUTE_EUROPE1

char msts_route[] = "EUROPE1" ;
char msts_pdb_file[] = "europe1.trk" ;
char msts_tdb_file[] = "settleca.tdb" ;
char msts_rdb_file[] = "SettleCa.rdb" ;

#elif defined ROUTE_EUROPE2

char msts_route[] = "EUROPE2" ;
char msts_pdb_file[] = "europe2.trk" ;
char msts_tdb_file[] = "orientx.tdb" ;
char msts_rdb_file[] = "orientx.rdb" ;

#elif defined ROUTE_TUTORIAL

char msts_route[] = "TUTORIAL ROUTE" ;
char msts_pdb_file[] = "tutorial route.trk" ;
char msts_tdb_file[] = "tutorial route.tdb" ;
char msts_rdb_file[] = "Tutorial Route.rdb" ;

#else

char msts_route[] = "MSTS_ROUTE_ERROR" ;

#endif

#endif
