/*
 *==============================================================================
 *345678901234567890123456789012345678901234567890123456789012345678901234567890
 *
 *   File:  setup_road.c
 *
 *   This file is part of ZR. Released under licence GPL-3.0-or-later.
 *   You should have received a copy of the GNU General Public License
 *   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
 *
 *   Routines used to generate road track shape files from the
 *     profile data
 *
 *   Based on OpenRails DynamicTrack.cs and related files
 *
 *==============================================================================
 */

TrkSector  *next_road_node(TrkSector *road_section) ;
int  add_sub_nodes_to_road(TrkSector *road_section, DynProfile *profile) ;
int   initialise_road_shape(ShapeNode *shapenode) ;

/*
 *  Routine:  make_default_road_profile()
 *
 *    Based on OR routine TrProfile().  This creates a "Default Road Profile"
 *    and adds it to the Dynamic Track List.
 *
 *    Indices incease from left to right
 *    X coordinate increases to right of track (Units m)
 *    Y coordinate increases upwards           (units m)
 */

int  make_default_road_profile(){

int            ip = 0          ;                    // Debug printing
DynProfile     *dyn_track  ;
TrackDistLevel *dist_level ;
TrackSubObject *sub_object    ;
RailProfile    *profile    ;
char            my_name[]="make_default_road_profile" ;

/*
 * Initialise new Dynamic Track Node
 */
      if(ip)printf(" Enter %s\n",my_name) ;
      dyn_track = (DynProfile *)malloc(sizeof(DynProfile)) ;
      if(zr_road_profile != NULL){
        dyn_track->next = zr_road_profile ;
      }else{
        dyn_track->next = NULL ;
      }
      zr_road_profile = dyn_track ;
/*
 *  Initialise Dynamic Track Structure
 */
      dyn_track->name          = strdup("Default Road Profile") ;
      dyn_track->lod_method    = COMPLETEREPLACEMENT ;
      dyn_track->pitch_control = CHORDLENGTH ;
      dyn_track->cangle_max    =  3.0        ;  // OR "ChordSpan" : maximum cord angle
      dyn_track->pitch_max     = 30.0        ;  // OR "PitchControlScalar" : Units m.
                                         // CHORDLENGTH       : maximum length of chord       30m
                                         // CHORDDISPLACEMENT : max displacement from circle   3cmm
      dyn_track->gauge         = 6.0     ;  //  Standard road width  6.0 m ??
      dyn_track->inner         = 0.0     ;  //  Not used
      dyn_track->outer         = 0.0     ;

      if(ip){
        printf("  Routine %s\n",my_name) ;
        printf("     gauge = %f\n",dyn_track->gauge) ;
      }
/*
 * Initialise distance levels
 */
      dyn_track->n_dist_levels  = 1 ;
      dyn_track->dist_level     = (TrackDistLevel *)malloc(1*sizeof(TrackDistLevel)) ;
/*
 * 1.  Distances to 1000000 m  (Nominal infinity)
 */
      dist_level                = &(dyn_track->dist_level [0]);
      dist_level->distance      = 1000000.0  ;
      dist_level->n_sub_objects = 1  ;
      dist_level->sub_object    = (TrackSubObject *)malloc(sizeof(TrackSubObject)) ;

      sub_object                = &(dist_level->sub_object[0]) ;
      sub_object->name          = strdup("RoadSurface") ;
      sub_object->texture_name  = strdup("DirtRoadTrans.ace") ;  //
      sub_object->shader_name   = strdup("TexDiff") ;
      sub_object->light_model   = strdup("OptSpecular0") ;
      sub_object->tex_add_mode_name = strdup("Wrap") ;
      sub_object->alpha_test    = 0     ;
      sub_object->ESD_alt_tex   = 0     ;
      sub_object->MipMap_LOD_Bias = 0   ;
      sub_object->n_profiles    = 1     ;
// Road profile structure is the same as for the rail profile
      sub_object->profile       = (RailProfile *)malloc(sizeof(RailProfile)) ;

/*
 *  Road
 */
      profile                   =&(sub_object->profile[0]) ;

      profile->delta_tex.U       = 0.0         ;
      profile->delta_tex.V       = 0.2088545   ;
      profile->name             = strdup("road") ;
      profile->n_elements       = 2           ;
      profile->vertex           = (Vector3 *)malloc(2*sizeof(Vector3)) ;
      profile->normal           = (Vector3 *)malloc(2*sizeof(Vector3)) ;
      profile->uvvector         = (UVVector2 *)malloc(2*sizeof(UVVector2)) ;

      profile->vertex[0].X     = -0.5*dyn_track->gauge ;  //  Distance right from centre line
      profile->vertex[0].Y     =  0.2 ;                         //  Height above base level
      profile->vertex[0].Z     =  0.0 ;                         //  Along track coordinate
      profile->normal[0].X     =  0.0 ;
      profile->normal[0].Y     =  1.0 ;                         //  Normal direction 'up'
      profile->normal[0].Z     =  0.0 ;
//      profile->uvvector[0].U   =  0.126953 ;
//      profile->uvvector[0].V   =  0.232067 ;
      profile->uvvector[0].U   =  0.0 ;
      profile->uvvector[0].V   =  1.0 ;

      profile->vertex[1].X     =  0.5*dyn_track->gauge ;  // Width ~ 5 m
      profile->vertex[1].Y     =  0.2 ;
      profile->vertex[1].Z     =  0.0 ;
      profile->normal[1].X     =  0.0 ;
      profile->normal[1].Y     =  1.0 ;
      profile->normal[1].Z     =  0.0 ;
//      profile->uvvector[1].U   =  0.224609 ;
//      profile->uvvector[1].V   =  0.232067 ;
      profile->uvvector[1].U   =  0.0 ;
      profile->uvvector[1].V   =  0.0 ;

      if(ip)printf("     road X = %f :: %f\n",profile->vertex[0].X,profile->vertex[1].X) ;


      return 0 ;
}

/*
 *   To prevent as much code duplication as possible, the roads here use the same data
 *   structures as the rail tracks but linked to the top level pointer 'road_db'
 *   instead of 'track_db'.
 */

int  make_road_shapes(){
uint i ;
int  ip = 0 ;                        // Debug printing = 1
TrkSector   *road_node       ;  // Road section needing shape
//DynProfile       *road_profile    ;  // Track Profile to use
char             my_name[]="make_road_shapes" ;

      if(ip)printf("  Enter routine %s\n",my_name) ;
      if(zr_road_profile == NULL){
        printf(" ERROR road_profile set to NULL\n");
        exit(0) ;
      }

//      road_profile = dyntracklist_beg ;
      for(i=0;i<road_db.trk_sections_array_size;i++){
        if(ip)printf("    make_road_shapes i = %i\n",i) ;
        road_node = &(road_db.trk_sections_array[i]) ;
        if(ip)printf("    make_road_shapes road_node    = %p\n",(void *)road_node) ;
        if(ip)printf("    make_road_shapes road_profile = %p\n",(void *)zr_road_profile) ;
        add_sub_nodes_to_road(road_node, zr_road_profile) ;
      }
      return 0 ;
}

/*
 *  Routine to add extra road nodes on curves.  Needed when using a road profile
 */

int  add_sub_nodes_to_road(TrkSector *road_section, DynProfile *profile){

uint          i, j, n   ;
int           ip = 0 ;                                // Debug printing = 1
double        x2, y2, r0, r1, a, b, w0, w1,
              da, xn, yn, tmp  ;
uint          n_road_nodes    ;   //  Number of vector nodes in section
uint          n_sub_nodes     ;   //  Number of sub nodes in road node
int           last_node = 0   ;   //  =1 if last node of section
double        x0, y0, z0,         //  Position of current node relative to tile
              xt0, yt0,           //  Tangent vector
              xn0, yn0,           //  Normal vector (left of road)
              x1, y1, z1,         //  Position of next node relative to tile
              xt1, yt1,           //  Tangent vector
              xn1, yn1,           //  Normal vector (left of road)
              a0,                 //  Tangent angle of current node
              a1,                 //  Tangent angle of next node
              xc, yc,             //  Weighted centre of circle
              aa ;
double        dcos = 1.0, dsin = 0.0,  //  Keep the optimising compiler happy
              xc0  = 0.0, yc0  = 0.0,  //  Centre of circles
              xc1  = 0.1, yc1  = 0.0  ;
TrkVectorNode *tv0,               //  Current road vector node
              *tv1  = NULL ;      //  Next road vector
TrkSector *ts1 = NULL ;      //  Next road section
char          *my_name="add_sub_nodes_to_road" ;

      if(ip)printf("\n    Enter routine %s\n",my_name) ;
      if(ip)printf("\n             ip = %i\n",ip) ;
      if(ip)printf("      Type of node = %i VECTOR =  %i END = %i.\n",
                 road_section->type_of_node,VECTOR_SECTION,END_SECTION);

/*
 * Skip if not a vector node (i.e. Junction or End Node)
 */
     if(road_section->type_of_node != VECTOR_SECTION){  //  See enum.h
       if(road_section->vector != NULL){
         road_section->vector[0].n_sub_nodes = 0 ;
       }
       return 1 ;
     }

      n_road_nodes = road_section->length_of_vector ;
      if(ip)printf("      Number of nodes = %i\n",n_road_nodes) ;
      for(i=0;i<n_road_nodes;i++){
        tv0 = &(road_section->vector[i])    ;
        if(i+1<n_road_nodes){
          tv1 = &(road_section->vector[i+1]);
          n_sub_nodes = 1                    ;
          last_node   = 0                    ;
        }else{
          ts1 = next_road_node(road_section)    ;  //  Find following node
          tv1 = &(ts1->vector[0])                ;  //  First vector of following node
          n_sub_nodes = 2                        ;  //  Will need an end sub-node
          last_node   = 1                        ;
          if(ip)printf(" New Section : ts0 = %p, ts1 = %p, tv0 = %p, tv1 = %p \n",
            (void *)road_section, (void *)ts1, (void *)tv0, (void *)tv1) ;
        }
        if(ip)printf("        tv0  : world tile %i %i :  tile %i %i\n",
          tv0->wfname_east_x,tv0->wfname_north_z,tv0->tile_east_x,tv0->tile_north_z) ;
        if(ip)printf("        tv1  : world tile %i %i :  tile %i %i\n",
          tv1->wfname_east_x,tv1->wfname_north_z,tv1->tile_east_x,tv1->tile_north_z) ;
        if(ip)printf("      Vector %i,    Pointers = %p, %p\n",i,(void *)tv0,(void *)tv1) ;

        x0 = tv0->east_x   ;  y0 = tv0->north_z  ;  z0 = tv0->height_y ;
        a0 = tv0->a_height_y ;  // Convert tangent angle to radians
        xt0 = sin(a0)      ; yt0 = cos(a0)       ;
        xn0 = -yt0         ; yn0 = xt0           ;
int ii, jj ;
        ii = tv1->tile_east_x  - tv0->tile_east_x  ;
        jj = tv1->tile_north_z - tv0->tile_north_z ;

        x1 = tv1->east_x   + ii * tile_size ;
        y1 = tv1->north_z  + jj * tile_size ;
//        x1 = tv1->east_x   ;
//        y1 = tv1->north_z  ;
        z1 = tv1->height_y ;
        a1 = tv1->a_height_y ;  // radians
//        printf("  CC1  a0 = %f %f\n",a0,a0*degree) ;
//        printf("  CC1  a1 = %f %f\n",a1,a1*degree) ;
        while(a1-a0 > pi)a1 = a1 - 2.0*pi ;
        while(a1-a0 <-pi)a1 = a1 + 2.0*pi ;
//        printf("  CC2  a0 = %f %f\n",a0,a0*degree) ;
//        printf("  CC2  a1 = %f %f\n",a1,a1*degree) ;

        if(fabs(a1-a0) > 1) {
          if(fabs(a1-a0) > 2.5){    // Assume connecting track in opposite direction
            a1 = a1 + pi ;
            while(a1-a0 > pi)a1 = a1 - 2.0*pi ;
            while(a1-a0 <-pi)a1 = a1 + 2.0*pi ;
          }else{
            printf(" Routine %s : Error : Angle between track sections > 1 radian.\n",my_name);
            printf("  Track section 0 = %i\n",road_section->uid) ;
            printf("   Angle a0 (deg) = %f\n",degree*tv0->a_height_y);
            if(last_node)printf("  Track section 1 = %i\n",ts1->uid) ;
            printf("   Angle a1 (deg) = %f\n",degree*tv1->a_height_y);
          }
        }
        xt1 = sin(a1)      ; yt1 = cos(a1)       ;
        xn1 = -yt1         ; yn1 = xt1           ;

        r0 = sqrt((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0)) ;  // Distance to next node
        x2  = x0 + xt0*r0   ; y2  = y0 + yt0*r0      ;  // Position if straight
        r1 = sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)) ;  // Error
        aa = r1/r0                                   ;  // Small angle approximation
        if(ip){
          printf("  CC  (x0, y0, z0) = %f %f %f,  a0 = %f\n",  x0,y0,z0,a0*degree) ;
          printf("  CC  (x1, y1, z1) = %f %f %f,  a1 = %f\n",  x1,y1,z1,a1*degree) ;
          printf("  CC   r0, r1      = %f %f\n", r0,r1) ;
          printf("  CC  (xt0, yt0  ) = %f %f\n", xt0,yt0) ;
          printf("  CC  (x2, y2    ) = %f %f      aa = %f\n", x2,y2,aa) ;
        }
/*
 *   Add extra nodes if track is curved.
 *
 *   Code assumes that : there is no banking of the track
 *                     : that the curvature is approximaly constant
 *                     : any errors in the tangent directions are small
 */
        n = 1 ;
        if(fabs(a0-a1) > radian*1.0 || aa > radian*1.0){            // Ignore errors < 1 degree

          r0 = ((x1-x0)*yn1 - (y1-y0)*xn1)/(xn0*yn1 - yn0*xn1)  ;  //  'Radius' at first node
          r1 = ((x0-x1)*yn0 - (y0-y1)*xn0)/(xn1*yn0 - yn1*xn0)  ;  //  'Radius' at next node
          xc0 = x0 + xn0*r0 ;    yc0 = y0 + yn0*r0 ;                 //  Centre of 'circle'
          xc1 = x1 + xn1*r1 ;    yc1 = y1 + yn1*r1 ;                 //  Centre of 'circle'
          if(ip)printf("  DD  r0 = %f,  r1 = %f,  xc0,yc0 = %f %f,  xc1,yc1 = %f %f\n",
                              r0,r1,xc0,yc0,xc1,yc1) ;
/*
 *  Calculate number of nodes
 */
          n = 1.0 + fabs(a1-a0)*degree/profile->cangle_max       ;  //  First estimate of number of sections
          aa = a1 - a0 ;                                           //  Angular change in direction (radian)
          if(aa > pi)aa = aa-2.0*pi ;
          if(aa <-pi)aa = aa+2.0*pi ;
          a = fabs(aa)/n                                         ;  //  First estimate of chord angle
          if(ip){
            printf("  DD  First estimate   n = %i\n",n) ;
            printf("    a0 = %f, a1 = %f, aa = %f  ::    %f %f %f\n",a0,a1,aa,a0*degree,a1*degree,aa*degree) ;
            printf("    pi = %f, degree = %f, radian = %f, cangle_max = %f\n",pi,degree,radian,profile->cangle_max);
          }

          if(profile->pitch_control == CHORDLENGTH){
            if(r0*a > profile->pitch_max)                   //  pitch_max == max length of chord
              n = 1 + fabs(r0*aa)/profile->pitch_max  ;
          }else{
            if(r0*(1.0-cos(a*0.5))>profile->pitch_max){    //  pitch_max == max distance from circle
              b = 2.0*acos(1.0 - profile->pitch_max/r0) ;
              n = a/b ;
            }
          }
//          n_sub_nodes = n_sub_nodes + n - 1 ;
          n_sub_nodes = n_sub_nodes + n;
          da = aa/n ;
          dcos = cos(da)   ;  dsin = sin(da) ;
          if(ip)printf("  DD  Second estimate  n = %i, n_sub_nodes = %i, last_node = %i, aa = %f, da = %f\n",
                 n,n_sub_nodes,last_node, aa, da) ;
        }
/*
 *  Allocate storage
 */
        n = n_sub_nodes  ;
        tv0->track_centre   = (Vector3 *)malloc(n*sizeof(Vector3)) ;
        tv0->left_vector    = (Vector3 *)malloc(n*sizeof(Vector3)) ;
        tv0->tangent_vector = (Vector3 *)malloc(n*sizeof(Vector3)) ;
        xn = xn0 ; yn = yn0 ;

        for(j=1;j<n_sub_nodes-last_node;j++){
          w1 = j*1.0/(n_sub_nodes-last_node) ;
          w0 = 1.0 - w1                      ;
          tmp = xn*dcos + yn*dsin ;           // Increment angle of normal vector
          yn  = yn*dcos - xn*dsin ;
          xn  = tmp               ;
          xc = w0*xc0 + w1*xc1    ;
          yc = w0*yc0 + w1*yc1    ;
          tv0->track_centre[j].X    =  xc - (w0*r0 + w1*r1)*xn ;
          tv0->track_centre[j].Y    =  yc - (w0*r0 + w1*r1)*yn ;
          tv0->track_centre[j].Z    =  w0*z0 + w1*z1           ;
          tv0->tangent_vector[j].X  =  yn  ;  // Unit vector
          tv0->tangent_vector[j].Y  = -xn  ;
          tv0->tangent_vector[j].Z  =  0.0 ;
          tv0->left_vector[j].X     =  xn  ;  // Unit vector
          tv0->left_vector[j].Y     =  yn  ;
          tv0->left_vector[j].Z     =  0.0 ;  //    No banking
          if(ip)printf("   DD1  j = %i, n_sub_nodes = %i, last_node = %i,  w0 = %f, w1 = %f\n",
                                j, n_sub_nodes, last_node, w0, w1) ;
          if(ip)printf("   DD2  %i %i : %f %f %f : %f %f %f : %f %f %f\n",i,j,
                tv0->track_centre[j].X, tv0->track_centre[j].Y, tv0->track_centre[j].Z,
                tv0->tangent_vector[j].X, tv0->tangent_vector[j].Y, tv0->tangent_vector[j].Z,
                tv0->left_vector[j].X, tv0->left_vector[j].Y, tv0->left_vector[j].Z) ;

        }
/*
 *  Initialise first sub- node
 */

        tv0->track_centre[0].X    =   x0 ;
        tv0->track_centre[0].Y    =   y0 ;
        tv0->track_centre[0].Z    =   z0 ;
        tv0->tangent_vector[0].X  =  xt0 ;  // Unit vector
        tv0->tangent_vector[0].Y  =  yt0 ;
        tv0->tangent_vector[0].Z  =  0.0 ;
        tv0->left_vector[0].X     =  xn0 ;  // Unit vector
        tv0->left_vector[0].Y     =  yn0 ;
        tv0->left_vector[0].Z     =  0.0 ;  //    No banking
        if(last_node){
          n = n_sub_nodes - 1 ;
          tv0->track_centre[n].X    =   x1 ;
          tv0->track_centre[n].Y    =   y1 ;
          tv0->track_centre[n].Z    =   z1 ;
          tv0->tangent_vector[n].X  =  xt0 ;  // Unit vector
          tv0->tangent_vector[n].Y  =  yt0 ;
          tv0->tangent_vector[n].Z  =  0.0 ;
          tv0->left_vector[n].X     =  xn0 ;  // Unit vector
          tv0->left_vector[n].Y     =  yn0 ;
          tv0->left_vector[n].Z     =  0.0 ;  //    No banking
          if(ip)printf(" FF Routine %s.  Last Node Initialised.\n",my_name);
        }
        tv0->n_sub_nodes = n_sub_nodes;
        if(ip)printf(" FF               n_sub_nodes = %i\n",n_sub_nodes);
      }

      if(ip){
        for(i=0;i<n_road_nodes;i++){
          tv0 = &(road_section->vector[i])    ;
          for(j=0;j<tv0->n_sub_nodes;j++){
            printf(" ZZ   %3i %3i : %8f %8f %8f : %8f %8f %8f : %8f %8f %f\n", i, j,
                  tv0->track_centre[j].X,  tv0->track_centre[j].Y,  tv0->track_centre[j].Z,
                  tv0->tangent_vector[j].X,tv0->tangent_vector[j].Y,tv0->tangent_vector[j].Z,
                  tv0->left_vector[j].X,   tv0->left_vector[j].Y,   tv0->left_vector[j].Z) ;
          }
        }
        printf("\n") ;
      }
      return 0 ;
}

/*
 *  Routine to find the next node.
 */

TrkSector  *next_road_node(TrkSector *road_section){

int             next_node_index, n ;
int             ip = 0 ;                      // Dubug printing when ip == 1
char            my_name[] = "next_road_node" ;
TrkSector  *t              ;

      if(ip){
        printf(" Enter routine %s\n",my_name) ;
        printf("  Type of Pin  = %i %i\n",road_section->type_of_pin[0],road_section->type_of_pin[1]);
        printf("  Index of Pin = %i %i %i\n",road_section->pin_to_section[0],
                                    road_section->pin_to_section[1],road_section->pin_to_section[2]);
      }
      n = road_section->type_of_pin[0] ;

      next_node_index = road_section->pin_to_section[n] ;
      if(next_node_index<0  || next_node_index > (int)road_db.trk_sections_array_size){
        printf(" Routine %s : Error :  Next track node index out of bounds\n",my_name) ;
        printf("   Node index = %i.\n",next_node_index)                    ;
        printf("   maximum value = %i\n",road_db.trk_sections_array_size) ;
        printf("   minimum value = 0\n")                                   ;
        printf("   Program stopping...\n")                                 ;
        exit(1) ;
      }

      t = &(road_db.trk_sections_array[next_node_index-1]) ;
      if(next_node_index != (int)t->uid){
        printf(" Routine %s : Error :  Unable to find next track section.\n",my_name) ;
        printf("   Index requested = %i\n",   next_node_index)         ;
        printf("   Index found     = %i\n",   t->uid)        ;
        printf("   Program stopping...\n")                             ;
        exit(1) ;
      }
      return t ;
}


/*
 *   Routine to make a standard shape file for each section of track
 *   using the default track profile.
 *
 *   Given the repeats in each rail section it may be better to plot
 *   using triangle strips rather than the triangles used by MSTS
 *   and OR.
 *
 *   Todo : Possible option - if there are no trilists then use
 *          triangle strips on the vertices at the sub-object level.
 */

int   make_road_shape(TrkSector *tracknode, DynProfile *profile){

int        i, n           ;
int        ip = 0         ;  // Debug printing
int        n_dist_levels  ;
ShapeNode  *shapenode     ;
LodControl *lod_control   ;
DistLevel  *dist_level    ;
TrackDistLevel *profile_dist_level      ;
char       string256[256] ;   //  Working string
char       my_name[]="make_road_shape" ;

      if(ip)printf(" Enter make_road_shape\n") ;

      shapenode = (ShapeNode *)malloc(sizeof(ShapeNode)) ;
      initialise_road_shape(shapenode) ;

      n = sprintf(string256,"TrkSect%06i",tracknode->uid);
      if(n>255){
        printf("  Buffer overflow in routine %s\n",my_name) ;
        exit(1) ;
      }
      shapenode->name             = strdup(string256) ;
      shapenode->n_lod_controls   = 1 ;
      shapenode->lod_control      =(LodControl *)malloc(sizeof(LodControl)) ;
      lod_control = shapenode->lod_control ;

      n_dist_levels               = profile->n_dist_levels    ;
      lod_control->dist_level     =(DistLevel *)
                                    malloc(n_dist_levels*sizeof(DistLevel)) ;
      for(i=0;i<n_dist_levels;i++){
        dist_level = &(lod_control->dist_level[i]) ;
        profile_dist_level = &(profile->dist_level[i])   ;

        dist_level->dlevel_selection  = profile_dist_level->distance  ;
        dist_level->n_hierarchy       = 0    ;
        dist_level->hierarchy         = NULL ;
        dist_level->n_sub_objects     = 0    ;
        dist_level->sub_object        = NULL ;
        dist_level->gl_display_list   = 0    ;
      }
      return 0 ;
}

int   initialise_road_shape(ShapeNode *shapenode){

      shapenode->next       = NULL ;
      shapenode->name       = NULL ;
      shapenode->s_file     = NULL ;
      shapenode->sd_file    = NULL ;
      shapenode->needed     = 0    ;  //  Needed for current scene
      shapenode->loaded     = 0    ;  //  Loaded in graphics card
//      shapenode->gl_display_list  = 0    ;
//      shapenode->gl_display_list2 = 0    ;
      shapenode->flags1     = 1    ;
      shapenode->flags2     = 1    ;
      shapenode->nvolumes   = 0    ;
      shapenode->shape_vol  = NULL ;
      shapenode->nshaders   = 0    ;
      shapenode->shader     = NULL ;
      shapenode->nfilters   = 0    ;
      shapenode->filter     = NULL ;
      shapenode->npoints    = 0    ;
      shapenode->point      = NULL ;
      shapenode->nnormals   = 0    ;
      shapenode->normal     = NULL ;
      shapenode->nuvpoints  = 0    ;
      shapenode->uvpoint    = NULL ;
      shapenode->nsort_vectors = 0    ;
      shapenode->sort_vector   = NULL ;
      shapenode->ncolors     = 0    ;
      shapenode->color       = NULL ;
      shapenode->nmatrices   = 0    ;
      shapenode->matrix      = NULL ;
      shapenode->n_textures       = 0    ;
      shapenode->texture_name     = NULL ;
      shapenode->texture          = NULL ;
      shapenode->use_texture = 0    ;
      shapenode->n_texlevel_low   = 0    ;
      shapenode->texlevel_low     = NULL ;
      shapenode->nlight_materials = 0    ;
      shapenode->light_material   = NULL ;
      shapenode->nlight_model_cfgs= 0    ;
      shapenode->light_model_cfg  = NULL ;
      shapenode->n_vtx_states     = 0    ;
      shapenode->vtx_state        = NULL ;
      shapenode->n_prim_states    = 0    ;
      shapenode->prim_state       = NULL ;

      shapenode->dlevel_selection = 0    ;
      shapenode->nhierarchy       = 0    ;
      shapenode->hierarchy        = NULL ;
      shapenode->n_lod_controls   = 0    ;
      shapenode->lod_control      = NULL ;
      shapenode->n_animations     = 0    ;
      shapenode->animation        = NULL ;
      shapenode->nhierarchy       = 0    ;
      shapenode->hierarchy        = NULL ;

      shapenode->esd_detail_level = 0 ;
      shapenode->esd_alternative_texture   = 0 ;
      shapenode->esd_no_visual_obstruction = 0 ;
      shapenode->esd_snapable        = 0 ;
      shapenode->esd_sound_file_name = NULL ;
      shapenode->esd_bell_animation_fps = 0.0 ;
      shapenode->esd_bounding_box_xmin = 0.0 ;
      shapenode->esd_bounding_box_xmax = 0.0 ;
      shapenode->esd_bounding_box_ymin = 0.0 ;
      shapenode->esd_bounding_box_ymax = 0.0 ;
      shapenode->esd_bounding_box_zmin = 0.0 ;
      shapenode->esd_bounding_box_zmax = 0.0 ;

      return 0;
}


