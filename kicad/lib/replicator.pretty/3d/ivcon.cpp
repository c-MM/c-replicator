//
//  Purpose:
//
//    IVCON converts various 3D graphics files.
//
//  Acknowledgements:
//
//    Coding, comments, and advice were supplied by a number of collaborators.
//
//    Jean-Cristophe Hoelt (hoeltj AT tcd.ie) pointed out that the program was
//    not compiling under GNU C++, and I made the necessary corrections.
//
//    John F Flanagan made some corrections to the 3D Studio Max routines.
//
//    Zik Saleeba (zik AT zikzak.net) enhanced the DXF routines, and added the
//    Golgotha GMOD routines.
//
//    Thanks to Susan M. Fisher, University of North Carolina,
//    Department of Computer Science, for pointing out a coding error
//    in FACE_NULL_DELETE that was overwriting all the data!
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 January 2004
//
//  Author:
//
//    John Burkardt
//
# include <cstdlib>
# include <cmath>
# include <cstdio>
# include <iostream>
# include <iomanip>
# include <fstream>
# include <cstring>

using namespace std;

# define ERROR 1
# define G1_SECTION_MODEL_QUADS 18
# define G1_SECTION_MODEL_TEXTURE_NAMES 19
# define G1_SECTION_MODEL_VERT_ANIMATION 20
# define GMOD_MAX_SECTIONS 32
# define GMOD_UNUSED_VERTEX 65535
# define PI 3.141592653589793238462643

# define DEG_TO_RAD   ( PI / 180.0 )
# define RAD_TO_DEG   ( 180.0 / PI )

//****************************************************************************80
//
//  GLOBAL DATA
//
//****************************************************************************80
//
//  BACKGROUND_RGB[3], the background color.
//
//  BYTE_SWAP, byte swapping option.
//
//  COR3[3][COR3_MAX], the coordinates of nodes.
//
//  COR3_MATERIAL[COR3_MAX], the index of the material of each node.
//
//  COR3_MAX, the maximum number of points.
//
//  COR3_NORMAL[3][COR3_MAX], normal vectors associated with nodes.
//
//  COR3_NUM, the number of points.
//
//  COR3_RGB[3][COR3_MAX], RGB colors associated with nodes.
//
//  COR3_TEX_UV[2][COR3_MAX], texture coordinates associated with nodes.
//
//  FACE[ORDER_MAX][FACE_MAX] contains the index of the I-th node making up face J.
//
//  FACE_AREA(FACE_MAX), the area of each face.
//
//  FACE_MATERIAL[FACE_MAX]; the material of each face.
//
//  FACE_MAX, the maximum number of faces.
//
//  FACE_NORMAL[3][FACE_MAX], the face normal vectors.
//
//  FACE_NUM, the number of faces.
//
//  FACE_ORDER[FACE_MAX], the number of vertices per face.
//
//  FACE_TEX_UV[2][FACE_MAX], texture coordinates associated with faces.
//
//  LINE_DEX[LINES_MAX], node indices, denoting polylines, each terminated by -1.
//
//  LINE_MATERIAL[LINES_MAX], index into RGBCOLOR for line color.
//
//  LINES_MAX, the maximum number of line definition items.
//
//  LINE_NUM, the number of line definition items.
//
//  LINE_PRUNE, pruning option ( 0 = no pruning, nonzero = pruning).
//
//  MATERIAL_MAX, the maximum number of materials.
//
//  MATERIAL_NUM, the number of materials.
//
//  ORDER_MAX, the maximum number of vertices per face.
//
//  TEXTURE_MAX, the maximum number of textures.
//
//  TEXTURE_NAME[TEXTURE_MAX][LINE_MAX_LEN], ...
//
//  TEXTURE_NUM, the number of textures.
//
//  TRANSFORM_MATRIX[4][4], the current transformation matrix.
//
//  VERTEX_MATERIAL[ORDER_MAX][FACE_MAX]; the material of vertices of faces.
//
//  VERTEX_NORMAL[3][ORDER_MAX][FACE_MAX], normals at vertices of faces.
//
//  VERTEX_RGB[3][ORDER_MAX][FACE_MAX], colors of vertices of faces.
//
//  VERTEX_TEX_UV[2][ORDER_MAX][FACE_MAX], texture coordinates of vertices of faces.
//

# define COLOR_MAX 1000
# define COR3_MAX 200000
# define FACE_MAX 200000
# define LINE_MAX_LEN 256
# define LEVEL_MAX 10
# define LINES_MAX 100000
# define MATERIAL_MAX 100
# define ORDER_MAX 10
# define TEXTURE_MAX 100

char anim_name[LINE_MAX_LEN];
float background_rgb[3];
int bad_num;
bool byte_swap;
int bytes_num;
int color_num;
int comment_num;

float cor3[3][COR3_MAX];
int cor3_material[COR3_MAX];
float cor3_normal[3][COR3_MAX];
int cor3_num;
float cor3_tex_uv[3][COR3_MAX];

bool debug;

int dup_num;

int face[ORDER_MAX][FACE_MAX];
float  face_area[FACE_MAX];
int    face_flags[FACE_MAX];
int    face_material[FACE_MAX];
float  face_normal[3][FACE_MAX];
int    face_num;
int    face_object[FACE_MAX];
int    face_order[FACE_MAX];
int    face_smooth[FACE_MAX];
float  face_tex_uv[2][FACE_MAX];

char   filein_name[81];
char   fileout_name[81];

int    group_num;

int    i;
char   input[LINE_MAX_LEN];
int    k;
char   level_name[LEVEL_MAX][LINE_MAX_LEN];

int    line_dex[LINES_MAX];
int    line_material[LINES_MAX];
int    line_num;
int    line_prune;

int    list[COR3_MAX];

char   material_binding[80];
char   material_name[MATERIAL_MAX][LINE_MAX_LEN];
int    material_num;
float  material_rgba[4][MATERIAL_MAX];

char   mat_name[81];
int    max_order2;

char   normal_binding[80];
float  normal_temp[3][ORDER_MAX*FACE_MAX];

char   object_name[81];
int    object_num;

float  origin[3];
float  pivot[3];
float  rgbcolor[3][COLOR_MAX];
char   temp_name[81];

int    text_num;

char texture_binding[80];
char texture_name[TEXTURE_MAX][LINE_MAX_LEN];
int texture_num;
float texture_temp[2][ORDER_MAX*FACE_MAX];

float transform_matrix[4][4];

int vertex_material[ORDER_MAX][FACE_MAX];
float  vertex_normal[3][ORDER_MAX][FACE_MAX];
float  vertex_rgb[3][ORDER_MAX][FACE_MAX];
float  vertex_tex_uv[2][ORDER_MAX][FACE_MAX];

//****************************************************************************80
//
//  FUNCTION PROTOTYPES
//
//****************************************************************************80

int main ( int argc, char *argv[] );
int ase_read ( FILE *filein );
int ase_write ( FILE *fileout );
int byu_read ( FILE *filein );
int byu_write ( FILE *fileout );
char ch_cap ( char c );
bool ch_eqi ( char c1, char c2 );
int ch_index_last ( char* string, char c );
bool ch_is_space ( char c );
int ch_pad ( int *char_index, int *null_index, char *s, int max_string );
char ch_read ( FILE *filein );
int ch_to_digit ( char c );
int ch_write ( FILE *fileout, char c );
int command_line ( char **argv );
void cor3_normal_set ( );
void cor3_range ( );
void data_check ( );
void data_init ( );
bool data_read ( );
void data_report ( );
int data_write ( );
int dxf_read ( FILE *filein );
int dxf_write ( FILE *fileout );
int edge_count ( );
void edge_null_delete ( );
void face_area_set ( );
void face_normal_ave ( );
void face_null_delete ( );
int face_print ( int iface );
void face_reverse_order ( );
int face_subset ( );
void face_to_line ( );
void face_to_vertex_material ( );
char *file_ext ( char *file_name );
float float_read ( FILE *filein );
float float_reverse_bytes ( float x );
int float_write ( FILE *fileout, float float_val );
bool gmod_arch_check ( );
int gmod_read ( FILE *filein );
float gmod_read_float ( FILE *filein );
unsigned short gmod_read_w16 ( FILE *filein );
unsigned long gmod_read_w32 ( FILE *filein );
int gmod_write ( FILE *fileout );
void gmod_write_float ( float Val, FILE *fileout );
void gmod_write_w16 ( unsigned short Val, FILE *fileout );
void gmod_write_w32 ( unsigned long Val, FILE *fileout );
void hello ( );
void help ( );
int hrc_read ( FILE *filein );
int hrc_write ( FILE *fileout );
int i4_max ( int i1, int i2 );
int i4_min ( int i1, int i2 );
int i4_modp ( int i, int j );
int i4_wrap ( int ival, int ilo, int ihi );
void init_program_data ( );
int interact ( );
int iv_read ( FILE *filein );
int iv_write ( FILE *fileout );
int i4vec_max ( int n, int *a );
long int long_int_read ( FILE *filein );
int long_int_write ( FILE *fileout, long int int_val );
void news ( );
void node_to_vertex_material ( );
int obj_read ( FILE *filein );
int obj_write ( FILE *fileout );
int off_read ( ifstream &file_in );
int off_write ( FILE *fileout );
int pov_write ( FILE *fileout );
int rcol_find ( float a[][COR3_MAX], int m, int n, float r[] );
float rgb_to_hue ( float r, float g, float b );
bool s_eqi ( char* string1, char* string2 );
int s_len_trim ( char *s );
int s_to_i4 ( char *s, int *last, bool *error );
bool s_to_i4vec ( char *s, int n, int ivec[] );
float s_to_r4 ( char *s, int *lchar, bool *error );
bool s_to_r4vec ( char *s, int n, float rvec[] );
short int short_int_read ( FILE *filein );
int short_int_write ( FILE *fileout, short int int_val );
int smf_read ( FILE *filein );
int smf_write ( FILE *fileout );
void sort_heap_external ( int n, int *indx, int *i, int *j, int isgn );
int stla_read ( FILE *filein );
int stla_write ( FILE *fileout );
int stlb_read ( FILE *filein );
int stlb_write ( FILE *fileout );
void tds_pre_process ( );
int tds_read ( FILE *filein );
unsigned long int tds_read_ambient_section ( FILE *filein );
unsigned long int tds_read_background_section ( FILE *filein );
unsigned long int tds_read_boolean ( unsigned char *boolean, FILE *filein );
unsigned long int tds_read_camera_section ( FILE *filein );
unsigned long int tds_read_edit_section ( FILE *filein, int *views_read );
unsigned long int tds_read_keyframe_section ( FILE *filein, int *views_read );
unsigned long int tds_read_keyframe_objdes_section ( FILE *filein );
unsigned long int tds_read_light_section ( FILE *filein );
unsigned long int tds_read_u_long_int ( FILE *filein );
int tds_read_long_name ( FILE *filein );
unsigned long int tds_read_matdef_section ( FILE *filein );
unsigned long int tds_read_material_section ( FILE *filein );
int tds_read_name ( FILE *filein );
unsigned long int tds_read_obj_section ( FILE *filein );
unsigned long int tds_read_object_section ( FILE *filein );
unsigned long int tds_read_tex_verts_section ( FILE *filein );
unsigned long int tds_read_texmap_section ( FILE *filein );
unsigned short int tds_read_u_short_int ( FILE *filein );
unsigned long int tds_read_spot_section ( FILE *filein );
unsigned long int tds_read_unknown_section ( FILE *filein );
unsigned long int tds_read_view_section ( FILE *filein, int *views_read );
unsigned long int tds_read_vp_section ( FILE *filein, int *views_read );
int tds_write ( FILE *fileout );
int tds_write_string ( FILE *fileout, char *string );
int tds_write_u_short_int ( FILE *fileout, unsigned short int int_val );
int tec_write ( FILE *fileout );
void tmat_init ( float a[4][4] );
void tmat_mxm ( float a[4][4], float b[4][4], float c[4][4] );
void tmat_mxp ( float a[4][4], float x[4], float y[4] );
void tmat_mxp2 ( float a[4][4], float x[][3], float y[][3], int n );
void tmat_mxv ( float a[4][4], float x[4], float y[4] );
void tmat_rot_axis ( float a[4][4], float b[4][4], float angle, char axis );
void tmat_rot_vector ( float a[4][4], float b[4][4], float angle,
  float v1, float v2, float v3 );
void tmat_scale ( float a[4][4], float b[4][4], float sx, float sy, float sz );
void tmat_shear ( float a[4][4], float b[4][4], char *axis, float s );
void tmat_trans ( float a[4][4], float b[4][4], float x, float y, float z );
int tria_read ( FILE *filein );
int tria_write ( FILE *fileout );
int trib_read ( FILE *filein );
int trib_write ( FILE *fileout );
int txt_write ( FILE *fileout );
int ucd_write ( FILE *fileout );
void vertex_normal_set ( );
void vertex_to_face_material ( );
void vertex_to_node_material ( );
int vla_read ( FILE *filein );
int vla_write ( FILE *fileout );
int wrl_write ( FILE *filout );
int xgl_write ( FILE *fileout );

//****************************************************************************80

int main ( int argc, char *argv[] )

//****************************************************************************80
//
//  Purpose:
//
//    MAIN is the main program for IVCON.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    05 September 2002
//
//  Author:
//
//    John Burkardt
//
{
  int result;

  cout << "\n";
  cout << "IVCON:\n";
  cout << "  C++ version\n";
  cout << "  Read, interpret, and write out graphics information\n";
  cout << "  in a variety of formats.\n";
  cout << "\n";
  cout << "  Compiled on " << __DATE__ << " at " << __TIME__ << "\n";
//
//  Initialize the program data.
//
  init_program_data ( );
//
//  If there are at least two command line arguments, call COMMAND_LINE.
//  Otherwise call INTERACT and get information from the user.
//
  if ( 2 <= argc )
  {
    result = command_line ( argv );
  }
  else
  {
    result = interact (  );
  }

  return result;
}
//****************************************************************************80

int ase_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    ASE_READ reads an AutoCAD ASE file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    22 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  float bval;
  int   count;
  float gval;
  int   i;
  int   iface;
  int   ivert;
  int   iword;
  int   level;
  char *next;
  int   nlbrack;
  int   nrbrack;
  int   cor3_num_old;
  int   face_num_old;
  float rval;
  float temp;
  int   width;
  char  word[LINE_MAX_LEN];
  char  word1[LINE_MAX_LEN];
  char  word2[LINE_MAX_LEN];
  char  wordm1[LINE_MAX_LEN];
  float x;
  float y;
  float z;

  level = 0;
  strcpy ( level_name[0], "Top" );
  cor3_num_old = cor3_num;
  face_num_old = face_num;
  nlbrack = 0;
  nrbrack = 0;

  strcpy ( word, " " );
  strcpy ( wordm1, " " );
//
//  Read a line of text from the file.
//
  for ( ;; )
  {
    if ( fgets ( input, LINE_MAX_LEN, filein ) == NULL )
    {
      break;
    }

    text_num = text_num + 1;
    next = input;
    iword = 0;
//
//  Read the next word from the line.
//
    for ( ;; )
    {

      strcpy ( wordm1, word );
      strcpy ( word, " " );

      count = sscanf ( next, "%s%n", word, &width );
      next = next + width;

      if ( count <= 0 )
      {
        break;
      }

      iword = iword + 1;

      if ( iword == 1 )
      {
        strcpy ( word1, word );
      }
//
//  In case the new word is a bracket, update the bracket count.
//
      if ( strcmp ( word, "{" ) == 0 )
      {
        nlbrack = nlbrack + 1;
        level = nlbrack - nrbrack;
        strcpy ( level_name[level], wordm1 );
      }
      else if ( strcmp ( word, "}" ) == 0 )
      {
        nrbrack = nrbrack + 1;

        if ( nlbrack < nrbrack )
        {
          cout << "\n";
          cout << "ASE_READ - Fatal error!\n";
          cout << "  Extraneous right bracket on line " << text_num << "\n";
          cout << "  Currently processing field:\n";
          cout << level_name[level] << "\n";;
          return 1;
        }

      }
//
//  *3DSMAX_ASCIIEXPORT  200
//
      if ( strcmp ( word1, "*3DSMAX_ASCIIEXPORT" ) == 0 )
      {
        break;
      }
//
//  *COMMENT
//
      else if ( strcmp ( word1, "*COMMENT" ) == 0 )
      {
        break;
      }
//
//  *GEOMOBJECT
//
      else if ( strcmp ( level_name[level], "*GEOMOBJECT" ) == 0 )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
          continue;
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
          continue;
        }
//
//  Why don't you read and save this name?
//
        else if ( strcmp ( word, "*NODE_NAME" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "*NODE_TM" ) == 0 )
        {
          continue;
        }
        else if ( strcmp ( word, "*MESH" ) == 0 )
        {
          continue;
        }
        else if ( strcmp ( word, "*PROP_CASTSHADOW" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "*PROP_MOTIONBLUR" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "*PROP_RECVSHADOW" ) == 0 )
        {
          break;
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "\n";
          cout << "ASE_READ - Error!\n";
          cout << "  Bad data in GEOMOBJECT, line " << text_num << "\n";
          break;
        }
      }
//
//  *MESH
//
      else if ( strcmp ( level_name[level], "*MESH" ) == 0 )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
          continue;
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
          continue;
        }
        else if ( strcmp ( word, "*MESH_CFACELIST" ) == 0 )
        {
          continue;
        }
        else if ( strcmp ( word, "*MESH_CVERTLIST" ) == 0 )
        {
          continue;
        }
        else if ( strcmp ( word, "*MESH_FACE_LIST" ) == 0 )
        {
          continue;
        }
        else if ( strcmp ( word, "*MESH_NORMALS" ) == 0 )
        {
          continue;
        }
        else if ( strcmp ( word, "*MESH_NUMCVERTEX" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "*MESH_NUMCVFACES" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "*MESH_NUMFACES" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "*MESH_NUMTVERTEX" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "*MESH_NUMTVFACES" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "*MESH_NUMVERTEX" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "*MESH_TFACELIST" ) == 0 )
        {
          continue;
        }
        else if ( strcmp ( word, "*MESH_TVERTLIST" ) == 0 )
        {
          continue;
        }
        else if ( strcmp ( word, "*MESH_VERTEX_LIST" ) == 0 )
        {
          continue;
        }
        else if ( strcmp ( word, "*TIMEVALUE" ) == 0 )
        {
          break;
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "Bad data in MESH, line " << text_num << "\n";
          break;
        }
      }
//
//  *MESH_CFACELIST
//
      else if ( strcmp ( level_name[level], "*MESH_CFACELIST" ) == 0 )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
          continue;
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
          continue;
        }
        else if ( strcmp ( word, "*MESH_CFACE" ) == 0 )
        {
          break;
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "Bad data in MESH_CFACE, line " << text_num << "\n";
          break;
        }
      }
//
//  *MESH_CVERTLIST
//
//  Mesh vertex indices must be incremented by COR3_NUM_OLD before being stored
//  in the internal array.
//
      else if ( strcmp ( level_name[level], "*MESH_CVERTLIST" ) == 0 )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
          continue;
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
          continue;
        }
        else if ( strcmp ( word, "*MESH_VERTCOL" ) == 0 )
        {
          count = sscanf ( next, "%d%n", &i, &width );
          next = next + width;

          i = i + cor3_num_old;

          count = sscanf ( next, "%f%n", &rval, &width );
          next = next + width;

          count = sscanf ( next, "%f%n", &gval, &width );
          next = next + width;

          count = sscanf ( next, "%f%n", &bval, &width );
          next = next + width;

          if ( material_num < MATERIAL_MAX )
          {
            material_rgba[0][material_num] = rval;
            material_rgba[1][material_num] = gval;
            material_rgba[2][material_num] = bval;
            material_rgba[3][material_num] = 1.0;
          }

          material_num = material_num + 1;
          cor3_material[i] = material_num;
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "\n";
          cout << "ASE_READ - Warning!\n";
          cout << "  Bad data in MESH_CVERTLIST, line " << text_num << "\n";
          break;
        }

      }
//
//  *MESH_FACE_LIST
//  This coding assumes a face is always triangular or quadrilateral.
//
      else if ( strcmp ( level_name[level], "*MESH_FACE_LIST" ) == 0 )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
          continue;
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
          continue;
        }
        else if ( strcmp ( word, "*MESH_FACE" ) == 0 )
        {
          if ( face_num < FACE_MAX )
          {

            face_material[face_num] = 0;
            face_order[face_num] = 0;

            count = sscanf ( next, "%d%n", &i, &width );
            next = next + width;

            count = sscanf ( next, "%s%n", word2, &width );
            next = next + width;
            count = sscanf ( next, "%s%n", word2, &width );
            next = next + width;

            count = sscanf ( next, "%d%n", &i, &width );
            next = next + width;
            face[0][face_num] = i + cor3_num_old;
            face_order[face_num] = face_order[face_num] + 1;

            count = sscanf ( next, "%s%n", word2, &width );
            next = next + width;

            count = sscanf ( next, "%d%n", &i, &width );
            next = next + width;
            face[1][face_num] = i + cor3_num_old;
            face_order[face_num] = face_order[face_num] + 1;

            count = sscanf ( next, "%s%n", word2, &width );
            next = next + width;

            count = sscanf ( next, "%d%n", &i, &width );
            next = next + width;
            face[2][face_num] = i + cor3_num_old;
            face_order[face_num] = face_order[face_num] + 1;

            count = sscanf ( next, "%s%n", word2, &width );
            next = next + width;

            if ( strcmp ( word2, "D:" ) == 0 )
            {
              count = sscanf ( next, "%d%n", &i, &width );
              next = next + width;
              face[3][face_num] = i + cor3_num_old;
              face_order[face_num] = face_order[face_num] + 1;
            }
          }

          face_num = face_num + 1;

          break;

        }
        else
        {
          bad_num = bad_num + 1;
          cout << "Bad data in MESH_FACE_LIST, line " << text_num << "\n";
          break;
        }
      }
//
//  *MESH_NORMALS
//
      else if ( strcmp ( level_name[level], "*MESH_NORMALS" ) == 0 )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
          continue;
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
          continue;
        }
        else if ( strcmp ( word, "*MESH_FACENORMAL" ) == 0 )
        {
          count = sscanf ( next, "%d%n", &iface, &width );
          next = next + width;

          count = sscanf ( next, "%f%n", &x, &width );
          next = next + width;

          count = sscanf ( next, "%f%n", &y, &width );
          next = next + width;

          count = sscanf ( next, "%f%n", &z, &width );
          next = next + width;

          iface = iface + face_num_old;
          ivert = 0;

          face_normal[0][iface] = x;
          face_normal[1][iface] = y;
          face_normal[2][iface] = z;

          break;

        }
        else if ( strcmp ( word, "*MESH_VERTEXNORMAL" ) == 0 )
        {
          count = sscanf ( next, "%d%n", &i, &width );
          next = next + width;

          count = sscanf ( next, "%f%n", &x, &width );
          next = next + width;

          count = sscanf ( next, "%f%n", &y, &width );
          next = next + width;

          count = sscanf ( next, "%f%n", &z, &width );
          next = next + width;

          vertex_normal[0][ivert][iface] = x;
          vertex_normal[1][ivert][iface] = y;
          vertex_normal[2][ivert][iface] = z;
          ivert = ivert + 1;

          break;
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "Bad data in MESH_NORMALS, line " << text_num << "\n";
          break;
        }
      }
//
//  *MESH_TFACELIST
//
      else if ( strcmp ( level_name[level], "*MESH_TFACELIST" ) == 0 )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
          continue;
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
          continue;
        }
        else if ( strcmp ( word1, "*MESH_TFACE" ) == 0 )
        {
          break;
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "Bad data in MESH_TFACE_LIST, line " << text_num << "\n";
          break;
        }
      }
//
//  *MESH_TVERTLIST
//
      else if ( strcmp ( level_name[level], "*MESH_TVERTLIST" ) == 0 )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
          continue;
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
          continue;
        }
        else if ( strcmp ( word1, "*MESH_TVERT" ) == 0  )
        {
          break;
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "Bad data in MESH_TVERTLIST, line " << text_num << "\n";
          break;
        }
      }
//
//  *MESH_VERTEX_LIST
//
      else if ( strcmp ( level_name[level], "*MESH_VERTEX_LIST" ) == 0 )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
          cor3_num_old = cor3_num;
          continue;
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
          continue;
        }
        else if ( strcmp ( word1, "*MESH_VERTEX" ) == 0 )
        {

          count = sscanf ( next, "%d%n", &i, &width );
          next = next + width;

          count = sscanf ( next, "%f%n", &x, &width );
          next = next + width;

          count = sscanf ( next, "%f%n", &y, &width );
          next = next + width;

          count = sscanf ( next, "%f%n", &z, &width );
          next = next + width;

          i = i + cor3_num_old;
          if ( cor3_num < i + 1 )
          {
            cor3_num = i + 1;
          }

          if ( i < COR3_MAX )
          {
            cor3[0][i] =
              transform_matrix[0][0] * x
            + transform_matrix[0][1] * y
            + transform_matrix[0][2] * z
            + transform_matrix[0][3];

            cor3[1][i] =
              transform_matrix[1][0] * x
            + transform_matrix[1][1] * y
            + transform_matrix[1][2] * z
            + transform_matrix[1][3];

            cor3[2][i] =
              transform_matrix[2][0] * x
            + transform_matrix[2][1] * y
            + transform_matrix[2][2] * z
            + transform_matrix[2][3];
          }

          break;
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "Bad data in MESH_VERTEX_LIST, line " << text_num << "\n";
          break;
        }
      }
//
//  *NODE_TM
//
//  Each node should start out with a default transformation matrix.
//
      else if ( strcmp ( level_name[level], "*NODE_TM" ) == 0 )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
          tmat_init ( transform_matrix );

          continue;
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
          continue;
        }
        else if ( strcmp ( word, "*INHERIT_POS" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "*INHERIT_ROT" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "*INHERIT_SCL" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "*NODE_NAME" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "*TM_POS" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "*TM_ROTANGLE" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "*TM_ROTAXIS" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "*TM_ROW0" ) == 0 )
        {
          count = sscanf ( next, "%f%n", &temp, &width );
          next = next + width;
          transform_matrix[0][0] = temp;

          count = sscanf ( next, "%f%n", &temp, &width );
          next = next + width;
          transform_matrix[1][0] = temp;

          count = sscanf ( next, "%f%n", &temp, &width );
          next = next + width;
          transform_matrix[2][0] = temp;

          break;
        }
        else if ( strcmp ( word, "*TM_ROW1" ) == 0 )
        {
          count = sscanf ( next, "%f%n", &temp, &width );
          next = next + width;
          transform_matrix[0][1] = temp;

          count = sscanf ( next, "%f%n", &temp, &width );
          next = next + width;
          transform_matrix[1][1] = temp;

          count = sscanf ( next, "%f%n", &temp, &width );
          next = next + width;
          transform_matrix[2][1] = temp;

          break;
        }
        else if ( strcmp ( word, "*TM_ROW2" ) == 0 )
        {
          count = sscanf ( next, "%f%n", &temp, &width );
          next = next + width;
          transform_matrix[0][2] = temp;

          count = sscanf ( next, "%f%n", &temp, &width );
          next = next + width;
          transform_matrix[1][2] = temp;

          count = sscanf ( next, "%f%n", &temp, &width );
          next = next + width;
          transform_matrix[2][2] = temp;

          break;
        }
        else if ( strcmp ( word, "*TM_ROW3" ) == 0 )
        {
          count = sscanf ( next, "%f%n", &temp, &width );
          next = next + width;
          transform_matrix[0][3] = temp;

          count = sscanf ( next, "%f%n", &temp, &width );
          next = next + width;
          transform_matrix[1][3] = temp;

          count = sscanf ( next, "%f%n", &temp, &width );
          next = next + width;
          transform_matrix[2][3] = temp;

          break;
        }
        else if ( strcmp ( word, "*TM_SCALE" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "*TM_SCALEAXIS" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "*TM_SCALEAXISANG" ) == 0 )
        {
          break;
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "Bad data in NODE_TM, line " << text_num << "\n";
          break;
        }
      }
//
//  *SCENE
//
      else if ( strcmp ( level_name[level], "*SCENE" ) == 0 )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
          continue;
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
          continue;
        }
        else if ( strcmp ( word, "*SCENE_AMBIENT_STATIC" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "*SCENE_BACKGROUND_STATIC" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "*SCENE_FILENAME" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "*SCENE_FIRSTFRAME" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "*SCENE_FRAMESPEED" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "*SCENE_LASTFRAME" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "*SCENE_TICKSPERFRAME" ) == 0 )
        {
          break;
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "Bad data in SCENE, line " << text_num << "\n";
          break;
        }
      }
    }
//
//  End of loop reading words from the line.
//
  }
//
//  End of loop reading lines from input file.
//

  return 0;
}
//****************************************************************************80

int ase_write ( FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    ASE_WRITE writes graphics information to an AutoCAD ASE file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    30 September 1998
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, FILE *fileout, the output file.
//
{
  int i1;
  int i2;
  int i3;
  int i4;
  int iface;
  int ivert;
  int j;
  int text_num;

  text_num = 0;
//
//  Write the header.
//
  fprintf ( fileout, "*3DSMAX_ASCIIEXPORT 200\n" );
  fprintf ( fileout, "*COMMENT \"%s, created by IVCON.\"\n", fileout_name );
  fprintf ( fileout, "*COMMENT \"Original data in %s\"\n", filein_name );

  text_num = text_num + 3;
//
//  Write the scene block.
//
  fprintf ( fileout, "*SCENE {\n" );
  fprintf ( fileout, "  *SCENE_FILENAME \"\"\n" );
  fprintf ( fileout, "  *SCENE_FIRSTFRAME 0\n" );
  fprintf ( fileout, "  *SCENE_LASTFRAME 100\n" );
  fprintf ( fileout, "  *SCENE_FRAMESPEED 30\n" );
  fprintf ( fileout, "  *SCENE_TICKSPERFRAME 160\n" );
  fprintf ( fileout, "  *SCENE_BACKGROUND_STATIC 0.0000 0.0000 0.0000\n" );
  fprintf ( fileout, "  *SCENE_AMBIENT_STATIC 0.0431 0.0431 0.0431\n" );
  fprintf ( fileout, "}\n" );

  text_num = text_num + 9;
//
//  Begin the big geometry block.
//
  fprintf ( fileout, "*GEOMOBJECT {\n" );
  fprintf ( fileout, "  *NODE_NAME \"%s\"\n", object_name );

  text_num = text_num + 2;
//
//  Sub block NODE_TM:
//
  fprintf ( fileout, "  *NODE_TM {\n" );
  fprintf ( fileout, "    *NODE_NAME \"Object01\"\n" );
  fprintf ( fileout, "    *INHERIT_POS 0 0 0\n" );
  fprintf ( fileout, "    *INHERIT_ROT 0 0 0\n" );
  fprintf ( fileout, "    *INHERIT_SCL 0 0 0\n" );
  fprintf ( fileout, "    *TM_ROW0 1.0000 0.0000 0.0000\n" );
  fprintf ( fileout, "    *TM_ROW1 0.0000 1.0000 0.0000\n" );
  fprintf ( fileout, "    *TM_ROW2 0.0000 0.0000 1.0000\n" );
  fprintf ( fileout, "    *TM_ROW3 0.0000 0.0000 0.0000\n" );
  fprintf ( fileout, "    *TM_POS 0.0000 0.0000 0.0000\n" );
  fprintf ( fileout, "    *TM_ROTAXIS 0.0000 0.0000 0.0000\n" );
  fprintf ( fileout, "    *TM_ROTANGLE 0.0000\n" );
  fprintf ( fileout, "    *TM_SCALE 1.0000 1.0000 1.0000\n" );
  fprintf ( fileout, "    *TM_SCALEAXIS 0.0000 0.0000 0.0000\n" );
  fprintf ( fileout, "    *TM_SCALEAXISANG 0.0000\n" );
  fprintf ( fileout, "  }\n" );

  text_num = text_num + 16;
//
//  Sub block MESH:
//    Items
//
  fprintf ( fileout, "  *MESH {\n" );
  fprintf ( fileout, "    *TIMEVALUE 0\n" );
  fprintf ( fileout, "    *MESH_NUMVERTEX %d\n", cor3_num );
  fprintf ( fileout, "    *MESH_NUMFACES %d\n", face_num );

  text_num = text_num + 4;
//
//  Sub sub block MESH_VERTEX_LIST
//
  fprintf ( fileout, "    *MESH_VERTEX_LIST {\n" );
  text_num = text_num + 1;

  for ( j = 0; j < cor3_num; j++ )
  {
    fprintf ( fileout, "      *MESH_VERTEX %d %f %f %f\n", j, cor3[0][j],
      cor3[1][j], cor3[2][j] );
    text_num = text_num + 1;
  }

  fprintf ( fileout, "    }\n" );
  text_num = text_num + 1;
//
//  Sub sub block MESH_FACE_LIST
//    Items MESH_FACE
//
  fprintf ( fileout, "    *MESH_FACE_LIST {\n" );
  text_num = text_num + 1;

  for ( iface = 0; iface < face_num; iface++ )
  {
    i1 = face[0][iface];
    i2 = face[1][iface];
    i3 = face[2][iface];

    if ( face_order[iface] == 3 )
    {
      fprintf ( fileout, "      *MESH_FACE %d: A: %d B: %d C: %d", iface, i1, i2, i3 );
      fprintf ( fileout, " AB: 1 BC: 1 CA: 1 *MESH_SMOOTHING *MESH_MTLID 1\n" );
      text_num = text_num + 1;
    }
    else if ( face_order[iface] == 4 )
    {
      i4 = face[3][iface];
      fprintf ( fileout, "      *MESH_FACE %d: A: %d B: %d C: %d D: %d", iface, i1, i2, i3, i4 );
      fprintf ( fileout, " AB: 1 BC: 1 CD: 1 DA: 1 *MESH_SMOOTHING *MESH_MTLID 1\n" );
      text_num = text_num + 1;
    }
  }

  fprintf ( fileout, "    }\n" );
  text_num = text_num + 1;
//
//  Item MESH_NUMTVERTEX.
//
  fprintf ( fileout, "    *MESH_NUMTVERTEX 0\n" );
  text_num = text_num + 1;
//
//  Item NUMCVERTEX.
//
  fprintf ( fileout, "    *MESH_NUMCVERTEX 0\n" );
  text_num = text_num + 1;
//
//  Sub block MESH_NORMALS
//    Items MESH_FACENORMAL, MESH_VERTEXNORMAL (repeated)
//
  fprintf ( fileout, "    *MESH_NORMALS {\n" );
  text_num = text_num + 1;

  for ( iface = 0; iface < face_num; iface++ )
  {
    fprintf ( fileout, "      *MESH_FACENORMAL %d %f %f %f\n",
      iface, face_normal[0][iface], face_normal[1][iface], face_normal[2][iface] );
    text_num = text_num + 1;

    for ( ivert = 0; ivert < face_order[iface]; ivert++ )
    {
      fprintf ( fileout, "      *MESH_VERTEXNORMAL %d %f %f %f\n",
        face[ivert][iface], vertex_normal[0][ivert][iface],
        vertex_normal[1][ivert][iface], vertex_normal[2][ivert][iface] );
      text_num = text_num + 1;
    }
  }

  fprintf ( fileout, "    }\n" );
  text_num = text_num + 1;
//
//  Close the MESH object.
//
  fprintf ( fileout, "  }\n" );
//
//  A few closing parameters.
//
  fprintf ( fileout, "  *PROP_MOTIONBLUR 0\n" );
  fprintf ( fileout, "  *PROP_CASTSHADOW 1\n" );
  fprintf ( fileout, "  *PROP_RECVSHADOW 1\n" );
//
//  Close the GEOM object.
//
  fprintf ( fileout, "}\n" );

  text_num = text_num + 5;
//
//  Report.
//
  cout << "\n";
  cout << "ASE_WRITE - Wrote " << text_num << " text lines;\n";

  return 0;
}
//****************************************************************************80

int byu_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    BYU_READ reads graphics data from a Movie.BYU surface geometry file.
//
//  Discussion:
//
//    A Movie.BYU surface geometry file contains 4 groups of data.
//
//    The first group of data is a single line, containing 4 integers,
//    each one left justified in 8 columns.  The integers are:
//
//      PART_NUM, VERTEX_NUM, POLY_NUM, EDGE_NUM,
//
//    that is, the number of parts or objects, the number of vertices or nodes,
//    the number of polygons or faces, and the number of edges.
//
//    The second group of data is a single line, containing 2 integers,
//    each one left justified in 8 columnes.  The integers are:
//
//      POLY1, POLY2,
//
//    the starting and ending polygon numbers.  Presumably, this means
//    that the polygons are labeled POLY1, POLY1+1, ..., POLY2, comprising
//    a total of POLY_NUM polygons.
//
//    The third group is the X, Y and Z coordinates of all the vertices.
//    These may be written using a FORTRAN format of 6E12.5, which
//    crams two sets of (X,Y,Z) data onto each line, with each real value
//    written in an exponential format with 5 places after the decimal.
//    However, it is generally possible to write the XYZ coordinate data
//    for each vertex on a separate line.
//
//    The fourth group defines the polygons in terms of the vertex indices.
//    For each polygon, the vertices that make up the polygon are listed in
//    counterclockwise order.  The last vertex listed is given with a negative
//    sign to indicate the end of the list.  All the vertices for all the
//    polygons are listed one after the other, using a format that puts
//    up to 10 left-justified integers on a line, with each integer occupying
//    8 spaces.
//
//    This code will certainly read a BYU file created by BYU_WRITE, but
//    it will not handle more general files.  In particular, an object
//    can have several parts, the coordinate data can be grouped so
//    that there are 2 sets of (x,y,z) data per line, and so on.
//
//  Example:
//
//      1       8       6      24
//      1       6
//    0.00000E+00 0.00000E+00 0.00000E+00
//    1.00000E+00 0.00000E+00 0.00000E+00
//    1.00000E+00 2.00000E+00 0.00000E+00
//    0.00000E+00 2.00000E+00 0.00000E+00
//    0.00000E+00 0.00000E+00 1.00000E+00
//    1.00000E+00 0.00000E+00 1.00000E+00
//    1.00000E+00 2.00000E+00 1.00000E+00
//    0.00000E+00 2.00000E+00 1.00000E+00
//      4       3       2      -1
//      5       6       7      -8
//      1       5       8      -4
//      4       8       7      -3
//      3       7       6      -2
//      2       6       5      -1
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    24 May 2001
//
//  Author:
//
//    John Burkardt
//
//
{
  int cor3_num_new;
  int count;
  int edge_num;
  int face_num_new;
  int iface;
  int ival;
  int ivert;
  int j;
  char *next;
  int part_num;
  int poly1;
  int poly2;
  int text_num;
  int width;
  float x;
  float y;
  float z;

  text_num = 0;

  if ( fgets ( input, LINE_MAX_LEN, filein ) == NULL )
  {
    return 1;
  }
  text_num = text_num + 1;

  sscanf ( input, "%d %d %d %d", &part_num, &cor3_num_new, &face_num_new,
    &edge_num );

  if ( fgets ( input, LINE_MAX_LEN, filein ) == NULL )
  {
    return 1;
  }
  text_num = text_num + 1;

  sscanf ( input, "%d %d", &poly1, &poly2 );

  for ( j = cor3_num; j < cor3_num + cor3_num_new; j++ )
  {
    if ( fgets ( input, LINE_MAX_LEN, filein ) == NULL )
    {
      return 1;
    }
    text_num = text_num + 1;

    sscanf ( input, "%f %f %f", &x, &y, &z );
    cor3[0][j] = x;
    cor3[1][j] = y;
    cor3[2][j] = z;
  }

  for ( iface = face_num; iface < face_num + face_num_new; iface++ )
  {
    if ( fgets ( input, LINE_MAX_LEN, filein ) == NULL )
    {
      return 1;
    }
    text_num = text_num + 1;

    next = input;
    ivert = 0;

    for (;;)
    {
      count = sscanf ( next, "%d%n", &ival, &width );
      next = next + width;

      if ( count <= 0 )
      {
        return 1;
      }

      if ( 0 < ival )
      {
        face[ivert][iface] = ival - 1 + cor3_num;
      }
      else
      {
        face[ivert][iface] = - ival - 1 - cor3_num;
        break;
      }

      ivert = ivert + 1;

    }
    face_order[iface] = ivert + 1;
  }

  cor3_num = cor3_num + cor3_num_new;
  face_num = face_num + face_num_new;
//
//  Report.
//
  cout << "\n";
  cout << "BYU_READ - Read " << text_num << " text lines.\n";

  return 0;
}
//****************************************************************************80

int byu_write ( FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    BYU_WRITE writes out the graphics data as a Movie.BYU surface geometry file.
//
//  Discussion:
//
//    A Movie.BYU surface geometry file contains 4 groups of data.
//
//    The first group of data is a single line, containing 4 integers,
//    each one left justified in 8 columns.  The integers are:
//
//      PART_NUM, VERTEX_NUM, POLY_NUM, EDGE_NUM,
//
//    that is, the number of parts or objects, the number of vertices or nodes,
//    the number of polygons or faces, and the number of edges.
//
//    The second group of data is a single line, containing 2 integers,
//    each one left justified in 8 columnes.  The integers are:
//
//      POLY1, POLY2,
//
//    the starting and ending polygon numbers.  Presumably, this means
//    that the polygons are labeled POLY1, POLY1+1, ..., POLY2, comprising
//    a total of POLY_NUM polygons.
//
//    The third group is the X, Y and Z coordinates of all the vertices.
//    These may be written using a FORTRAN format of 6E12.5, which
//    crams two sets of (X,Y,Z) data onto each line, with each real value
//    written in an exponential format with 5 places after the decimal.
//    However, it is generally possible to write the XYZ coordinate data
//    for each vertex on a separate line.
//
//    The fourth group defines the polygons in terms of the vertex indices.
//    For each polygon, the vertices that make up the polygon are listed in
//    counterclockwise order.  The last vertex listed is given with a negative
//    sign to indicate the end of the list.  All the vertices for all the
//    polygons are listed one after the other, using a format that puts
//    up to 10 left-justified integers on a line, with each integer occupying
//    8 spaces.
//
//  Example:
//
//      1       8       6      24
//      1       6
//    0.00000E+00 0.00000E+00 0.00000E+00
//    1.00000E+00 0.00000E+00 0.00000E+00
//    1.00000E+00 2.00000E+00 0.00000E+00
//    0.00000E+00 2.00000E+00 0.00000E+00
//    0.00000E+00 0.00000E+00 1.00000E+00
//    1.00000E+00 0.00000E+00 1.00000E+00
//    1.00000E+00 2.00000E+00 1.00000E+00
//    0.00000E+00 2.00000E+00 1.00000E+00
//      4       3       2      -1
//      5       6       7      -8
//      1       5       8      -4
//      4       8       7      -3
//      3       7       6      -2
//      2       6       5      -1
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    24 May 2001
//
//  Author:
//
//    John Burkardt
//
{
  int edge_num;
  int iface;
  int ivert;
  int j;
  int jp;
  int part_num;
  int text_num;

  text_num = 0;

  edge_num = 0;
  for ( iface = 0; iface < face_num; iface++ )
  {
    edge_num = edge_num + face_order[iface];
  }

  part_num = 1;

  fprintf ( fileout, "%d %d %d %d\n", part_num, cor3_num, face_num, edge_num );
  text_num = text_num + 1;

  fprintf ( fileout, "1 %d\n", face_num );
  text_num = text_num + 1;

  for ( j = 0; j < cor3_num; j++ )
  {
    fprintf ( fileout, "%f %f %f\n", cor3[0][j], cor3[1][j], cor3[2][j] );
    text_num = text_num + 1;
  }

  for ( iface = 0; iface < face_num; iface++ )
  {
    for ( ivert = 0; ivert < face_order[iface]; ivert++ )
    {
      jp = face[ivert][iface] + 1;
      if ( ivert == face_order[iface] - 1 )
      {
        jp = - jp;
      }
     fprintf ( fileout, "%d ", jp );
    }
    fprintf ( fileout, "\n" );
    text_num = text_num + 1;
  }
//
//  Report.
//
  cout << "\n";
  cout << "BYU_WRITE - Wrote " << text_num << " text lines.\n";

  return 0;
}
//****************************************************************************80

char ch_cap ( char c )

//****************************************************************************80
//
//  Purpose:
//
//    CH_CAP capitalizes a single character.
//
//  Discussion:
//
//    This routine should be equivalent to the library "toupper" function.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 July 1998
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, char C, the character to capitalize.
//
//    Output, char CH_CAP, the capitalized character.
//
{
  if ( 97 <= c && c <= 122 )
  {
    c = c - 32;
  }

  return c;
}
//****************************************************************************80

bool ch_eqi ( char c1, char c2 )

//****************************************************************************80
//
//  Purpose:
//
//    CH_EQI is true if two characters are equal, disregarding case.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    13 June 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, char C1, char C2, the characters to compare.
//
//    Output, bool CH_EQI, is true if the two characters are equal,
//    disregarding case.
//
{
  if ( 97 <= c1 && c1 <= 122 )
  {
    c1 = c1 - 32;
  }
  if ( 97 <= c2 && c2 <= 122 )
  {
    c2 = c2 - 32;
  }

  return ( c1 == c2 );
}
//****************************************************************************80

int ch_index_last ( char *s, char c )

//****************************************************************************80
//
//  Purpose:
//
//    CH_INDEX_LAST finds the last occurrence of a character in a string.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    16 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, char *S, a pointer to a string to be searched.
//
//    Input, char C, the character to be searched for in s.
//
//    Output, int CH_INDEX_LAST, the index in s of the last occurrence
//    of C, or -1 if c does not occur in s.
//
{
  int i;
  int j;
  int nchar;

  j = -1;

  nchar = strlen ( s );

  for ( i = 0; i < nchar; i++ )
  {
    if ( s[i] == c )
    {
      j = i;
    }
  }

  return j;

}
//****************************************************************************80

bool ch_is_space ( char c )

//****************************************************************************80
//
//  Purpose:
//
//    CH_IS_SPACE is TRUE if a character represents "white space".
//
//  Discussion:
//
//    A white space character is a space, a form feed, a newline, a carriage
//    return, a horizontal tab, or a vertical tab.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    02 October 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, char C, the character to be analyzed.
//
//    Output, bool CH_IS_SPACE, is TRUE if C is a whitespace character.
//
{
  if ( c == ' ' )
  {
    return true;
  }
  else if ( c == '\f' )
  {
    return true;
  }
  else if ( c == '\n' )
  {
    return true;
  }
  else if ( c == '\r' )
  {
    return true;
  }
  else if ( c == '\t' )
  {
    return true;
  }
  else if ( c == '\v' )
  {
    return true;
  }
  else
  {
    return false;
  }
}
//****************************************************************************80

int ch_pad ( int *char_index, int *null_index, char *s, int max_string )

//****************************************************************************80
//
//  Purpose:
//
//    CH_PAD "pads" a character in a string with a blank on either side.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    16 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input/output, int *CHAR_INDEX, the position of the character to be
//    padded.  On output, this is increased by 1.
//
//    Input/output, int *NULL_INDEX, the position of the terminating NULL in
//    the string.  On output, this is increased by 2.
//
//    Input/output, char S[MAX_STRING], the string to be manipulated.
//
//    Input, int MAX_STRING, the maximum number of characters that can be stored
//    in s.
//
//    Output, int CH_PAD, is 0 if the operation worked, and 1 otherwise.
//
{
  int i;

  if ( *char_index < 0 ||
      *null_index <= *char_index ||
      max_string - 1 < *char_index )
  {
    return 1;
  }

  if ( max_string - 1 < (*null_index) + 2 )
  {
    return 1;
  }

  for ( i = *null_index + 2; *char_index + 2 < i; i-- )
  {
    s[i] = s[i-2];
  }
  s[*char_index+2] = ' ';
  s[*char_index+1] = s[*char_index];
  s[*char_index] = ' ';

  *char_index = *char_index + 1;
  *null_index = *null_index + 2;

  return 0;
}
//****************************************************************************80

char ch_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    CH_READ reads one character from a binary file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    24 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  char c;

  c = ( char ) fgetc ( filein );

  return c;
}
//****************************************************************************80

int ch_to_digit ( char c )

//****************************************************************************80
//
//  Purpose:
//
//    CH_TO_DIGIT returns the integer value of a base 10 digit.
//
//  Example:
//
//     C   DIGIT
//    ---  -----
//    '0'    0
//    '1'    1
//    ...  ...
//    '9'    9
//    ' '    0
//    'X'   -1
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    13 June 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, char C, the decimal digit, '0' through '9' or blank are legal.
//
//    Output, int CH_TO_DIGIT, the corresponding integer value.  If C was
//    'illegal', then DIGIT is -1.
//
{
  int digit;

  if ( '0' <= c && c <= '9' )
  {
    digit = c - '0';
  }
  else if ( c == ' ' )
  {
    digit = 0;
  }
  else
  {
    digit = -1;
  }

  return digit;
}
//****************************************************************************80

int ch_write ( FILE *fileout, char c )

//****************************************************************************80
//
//  Purpose:
//
//    CH_WRITE writes one character to a binary file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    24 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  fputc ( c, fileout );

  return 1;
}
//****************************************************************************80

int command_line ( char **argv )

//****************************************************************************80
//
//  Purpose:
//
//    COMMAND_LINE carries out a command-line session of file conversion.
//
//  Discussion:
//
//    This routine is invoked when the user command is something like
//
//      ivcon filein_name fileout_name
//
//    or
//
//      ivcon -rn filein_name fileout_name
//
//    where "-rn" signals the "reverse normals" option, or
//
//      ivcon -rf filein_name fileout_name
//
//    where "-rf" signals the "reverse faces" option.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    28 June 1999
//
//  Author:
//
//    John Burkardt
//
{
  int i;
  int iarg;
  int icor3;
  int ierror;
  int iface;
  int ivert;
  bool reverse_faces;
  bool reverse_normals;
  bool success;
//
//  Initialize local data.
//
  iarg = 0;
  ierror = 0;
  reverse_faces = false;
  reverse_normals = false;
//
//  Initialize the graphics data.
//
  data_init ( );
//
//  Get the -RN option, -RF option, and the input file name.
//
  iarg = iarg + 1;
  strcpy ( filein_name, argv[iarg] );

  if ( s_eqi ( filein_name, "-RN" ) )
  {
    reverse_normals = true;
    cout << "\n";
    cout << "COMMAND_LINE: Reverse_Normals option requested.\n";
    iarg = iarg + 1;
    strcpy ( filein_name, argv[iarg] );
  }

  if ( s_eqi ( filein_name, "-RF" ) )
  {
    reverse_faces = true;
    cout << "\n";
    cout << "COMMAND_LINE: Reverse_Faces option requested.\n";
    iarg = iarg + 1;
    strcpy ( filein_name, argv[iarg] );
  }
//
//  Read the input.
//
  success = data_read ( );

  if ( !success )
  {
    cout << "\n";
    cout << "COMMAND_LINE - Fatal error!\n";
    cout << "  Failure reported from DATA_READ.\n";
    return 1;
  }
//
//  Reverse the normal vectors if requested.
//
  if ( reverse_normals )
  {

    for ( icor3 = 0; icor3 < cor3_num; icor3++ )
    {
      for ( i = 0; i < 3; i++ )
      {
        cor3_normal[i][icor3] = - cor3_normal[i][icor3];
      }
    }

    for ( iface = 0; iface < face_num; iface++ )
    {
      for ( i = 0; i < 3; i++ )
      {
        face_normal[i][iface] = - face_normal[i][iface];
      }
    }

    for ( iface = 0; iface < face_num; iface++ )
    {
      for ( ivert = 0; ivert < face_order[iface]; ivert++ )
      {
        for ( i = 0; i < 3; i++ )
        {
          vertex_normal[i][ivert][iface] =
            - vertex_normal[i][ivert][iface];
        }
      }
    }
    cout << "\n";
    cout << "COMMAND_LINE - Note:\n";
    cout << "  Reversed node, face, and vertex normals.\n";
  }
//
//  Reverse the faces if requested.
//
  if ( reverse_faces )
  {
    face_reverse_order ( );

    cout << "\n";
    cout << "COMMAND_LINE - Note:\n";
    cout << "  Reversed the face definitions.\n";
  }
//
//  Write the output file.
//
  iarg = iarg + 1;
  strcpy ( fileout_name, argv[iarg] );

  ierror = data_write ( );

  if ( ierror == 1 )
  {
    cout << "\n";
    cout << "COMMAND_LINE - Fatal error!\n";
    cout << "  Failure while writing output data.\n";
    return 1;
  }
  return 0;
}
//****************************************************************************80

void cor3_normal_set ( )

//****************************************************************************80
//
//  Purpose:
//
//    COR3_NORMAL_SET computes node normal vectors.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    18 November 1998
//
//  Author:
//
//    John Burkardt
//
{
  int   icor3;
  int   iface;
  int   ivert;
  int   j;
  float norm;
  float temp;

  for ( icor3 = 0; icor3 < cor3_num; icor3++ )
  {
    for ( j = 0; j < 3; j++ )
    {
      cor3_normal[j][icor3] = 0.0;
    }
  }
//
//  Add up the normals at all the faces to which the node belongs.
//
  for ( iface = 0; iface < face_num; iface++ )
  {
    for ( ivert = 0; ivert < face_order[iface]; ivert++ )
    {
      icor3 = face[ivert][iface];
      for ( j = 0; j < 3; j++ )
      {
        cor3_normal[j][icor3] = cor3_normal[j][icor3]
          + vertex_normal[j][ivert][iface];
      }
    }
  }
//
//  Renormalize.
//
  for ( icor3 = 0; icor3 < cor3_num; icor3++ )
  {
    norm = 0.0;
    for ( j = 0; j < 3; j++ )
    {
      temp = cor3_normal[j][icor3];
      norm = norm + temp * temp;
    }

    if ( norm == 0.0 )
    {
      norm = 3.0;
      for ( j = 0; j < 3; j++ )
      {
        cor3_normal[j][icor3] = 1.0;
      }
    }

    norm = ( float ) sqrt ( norm );

    for ( j = 0; j < 3; j++ )
    {
      cor3_normal[j][icor3] = cor3_normal[j][icor3] / norm;
    }
  }

  return;
}
//****************************************************************************80

void cor3_range ( )

//****************************************************************************80
//
//  Purpose:
//
//    COR3_RANGE computes the coordinate minima and maxima.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    31 August 1998
//
//  Author:
//
//    John Burkardt
//
{
  int   i;
  float xave;
  float xmax;
  float xmin;
  float yave;
  float ymax;
  float ymin;
  float zave;
  float zmax;
  float zmin;

  xave = cor3[0][0];
  xmax = cor3[0][0];
  xmin = cor3[0][0];

  yave = cor3[1][0];
  ymax = cor3[1][0];
  ymin = cor3[1][0];

  zave = cor3[2][0];
  zmax = cor3[2][0];
  zmin = cor3[2][0];

  for ( i = 1; i < cor3_num; i++ )
  {
    xave = xave + cor3[0][i];
    if ( cor3[0][i] < xmin )
    {
      xmin = cor3[0][i];
    }
    if ( xmax < cor3[0][i] )
    {
      xmax = cor3[0][i];
    }

    yave = yave + cor3[1][i];
    if ( cor3[1][i] < ymin )
    {
      ymin = cor3[1][i];
    }
    if ( ymax < cor3[1][i] )
    {
      ymax = cor3[1][i];
    }

    zave = zave + cor3[2][i];
    if ( cor3[2][i] < zmin )
    {
      zmin = cor3[2][i];
    }
    if ( zmax < cor3[2][i] )
    {
      zmax = cor3[2][i];
    }
  }

  xave = xave / cor3_num;
  yave = yave / cor3_num;
  zave = zave / cor3_num;

  cout << "\n";
  cout << "COR3_RANGE - Data range:\n";
  cout << "\n";
  cout << "   Minimum   Average   Maximum  Range\n";
  cout << "\n";
  cout << "X  " << setw(10) << xmin        << "  "
                << setw(10) << xave        << "  "
                << setw(10) << xmax        << "  "
                << setw(10) << xmax - xmin << "\n";
  cout << "Y  " << setw(10) << ymin        << "  "
                << setw(10) << yave        << "  "
                << setw(10) << ymax        << "  "
                << setw(10) << ymax - ymin << "\n";
  cout << "Z  " << setw(10) << zmin        << "  "
                << setw(10) << zave        << "  "
                << setw(10) << zmax        << "  "
                << setw(10) << zmax - zmin << "\n";

  return;
}
//****************************************************************************80

void data_check ( )

//****************************************************************************80
//
//  Purpose:
//
//    DATA_CHECK checks the input data.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    02 May 2005
//
//  Author:
//
//    John Burkardt
//
{
  int iface;
  int nfix;

  if ( COLOR_MAX < color_num )
  {
    cout << "\n";
    cout << "DATA_CHECK - Warning!\n";
    cout << "  The input data requires " << color_num << " colors.\n";
    cout << "  There was only room for " << COLOR_MAX << "\n";
    cout << "\n";
    cout << "  To correct this problem, you can change the internal\n";
    cout << "  value of COLOR_MAX, recompile, and rerun the program.\n";
    color_num = COLOR_MAX;
  }

  if ( COR3_MAX < cor3_num )
  {
    cout << "\n";
    cout << "DATA_CHECK - Warning!\n";
    cout << "  The input data requires " << cor3_num << " points.\n";
    cout << "  There was only room for " << COR3_MAX << "\n";
    cout << "\n";
    cout << "  To correct this problem, you can change the internal\n";
    cout << "  value of COR3_MAX, recompile, and rerun the program.\n";
    cor3_num = COR3_MAX;
  }

  if ( FACE_MAX < face_num )
  {
    cout << "\n";
    cout << "DATA_CHECK - Warning!\n";
    cout << "  The input data requires " << face_num << " faces.\n";
    cout << "  There was only room for " << FACE_MAX << "\n";
    cout << "\n";
    cout << "  To correct this problem, you can change the internal\n";
    cout << "  value of FACE_MAX, recompile, and rerun the program.\n";
    face_num = FACE_MAX;
  }

  if ( LINES_MAX < line_num )
  {
    cout << "\n";
    cout << "DATA_CHECK - Warning!\n";
    cout << "  The input data requires " << line_num << " line items.\n";
    cout << "  There was only room for " << LINES_MAX << ".\n";
    cout << "\n";
    cout << "  To correct this problem, you can change the internal\n";
    cout << "  value of LINES_MAX, recompile, and rerun the program.\n";
    line_num = LINES_MAX;
  }

  nfix = 0;

  for ( iface = 0; iface < face_num; iface++ )
  {
    if ( ORDER_MAX < face_order[iface] )
    {
      face_order[iface] = ORDER_MAX;
      nfix = nfix + 1;
    }
  }

  if ( 0 < nfix )
  {
    cout << "\n";
    cout << "DATA_CHECK - Warning!\n";
    cout << "  Corrected " << nfix
         << " faces using more than " << ORDER_MAX
         << " vertices per face.\n";
  }

  for ( i = 0; i < material_num; i++ )
  {
    if ( strcmp ( material_name[i], "" ) == 0 )
    {
      strcpy ( material_name[i], "Material_0000" );
    }
  }

  for ( i = 0; i < texture_num; i++ )
  {
    if ( strcmp ( texture_name[i], "" ) == 0 )
    {
      strcpy ( texture_name[i], "Texture_0000" );
    }
  }

  cout << "\n";
  cout << "DATA_CHECK - Data checked.\n";

  return;
}
//****************************************************************************80

void data_init ( )

//****************************************************************************80
//
//  Purpose:
//
//    DATA_INIT initializes the internal graphics data.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    04 July 2000
//
//  Author:
//
//    John Burkardt
//
{
  int i;
  int iface;
  int ivert;
  int j;
  int k;

  strcpy( anim_name, "" );

  for ( i = 0; i < 3; i++ )
  {
    background_rgb[i] = 0.0;
  }

  for ( i = 0; i < 3; i++ )
  {
    for ( j = 0; j < COR3_MAX; j++ )
    {
      cor3[i][j] = 0.0;
    }
  }

  for ( i = 0; i < COR3_MAX; i++ )
  {
    cor3_material[i] = 0;
  }

  for ( i = 0; i < 3; i++ )
  {
    for ( j = 0; j < COR3_MAX; j++ )
    {
      cor3_normal[i][j] = 0.0;
    }
  }

  for ( j = 0; j < COR3_MAX; j++ )
  {
    cor3_tex_uv[0][j] = 0.0;
    cor3_tex_uv[1][j] = 0.0;
  }

  for ( iface = 0; iface < FACE_MAX; iface++ )
  {
    for ( ivert = 0; ivert < ORDER_MAX; ivert++ )
    {
      face[ivert][iface] = 0;
    }
  }

  for ( iface = 0; iface < FACE_MAX; iface++ )
  {
    face_flags[iface] = 6;
  }

  for ( iface = 0; iface < FACE_MAX; iface++ )
  {
    face_material[iface] = 0;
  }

  for ( iface = 0; iface < FACE_MAX; iface++ )
  {
    for ( i = 0; i < 3; i++ )
    {
      face_normal[i][iface] = 0;
    }
  }

  for ( iface = 0; iface < FACE_MAX; iface++ )
  {
    face_object[iface] = -1;
  }

  for ( iface = 0; iface < FACE_MAX; iface++ )
  {
    face_order[iface] = 0;
  }

  for ( iface = 0; iface < FACE_MAX; iface++ )
  {
    face_smooth[iface] = 1;
  }

  for ( i = 0; i < LINES_MAX; i++ )
  {
    line_dex[i] = -1;
  }

  for ( i = 0; i < LINES_MAX; i++ )
  {
    line_material[i] = 0;
  }

  strcpy ( material_binding, "DEFAULT" );

  for ( j = 0; j < MATERIAL_MAX; j++ )
  {
    strcpy ( material_name[j], "Material_0000" );
  }

  for ( i = 0; i < 4; i++ )
  {
    for ( j = 0; j < MATERIAL_MAX; j++ )
    {
      material_rgba[i][j] = 0.0;
    }
  }

  strcpy ( normal_binding, "DEFAULT" );

  for ( j = 0; j < ORDER_MAX*FACE_MAX; j++ )
  {
    for ( i = 0; i < 3; i++ )
    {
      normal_temp[i][j] = 0;
    }
  }

  color_num = 0;
  cor3_num = 0;
  face_num = 0;
  group_num = 0;
  line_num = 0;
  material_num = 0;
  object_num = 0;
  texture_num = 0;

  strcpy ( object_name, "IVCON" );

  for ( i = 0; i < 3; i++ )
  {
    origin[i] = 0.0;
  }

  for ( i = 0; i < 3; i++ )
  {
    pivot[i] = 0.0;
  }

  for ( j = 0; j < COLOR_MAX; j++ )
  {
    rgbcolor[0][j] = 0.299;
    rgbcolor[1][j] = 0.587;
    rgbcolor[2][j] = 0.114;
  }

  strcpy ( texture_binding, "DEFAULT" );

  for ( j = 0; j < TEXTURE_MAX; j++ )
  {
    strcpy ( texture_name[j], "Texture_0000" );
  }

  tmat_init ( transform_matrix );

  for ( iface = 0; iface < FACE_MAX; iface++ )
  {
    for ( ivert = 0; ivert < ORDER_MAX; ivert++ )
    {
      vertex_material[ivert][iface] = 0;
    }
  }

  for ( iface = 0; iface < FACE_MAX; iface++ )
  {
    for ( ivert = 0; ivert < ORDER_MAX; ivert++ )
    {
      for ( i = 0; i < 3; i++ )
      {
        vertex_normal[i][ivert][iface] = 0.0;
      }
    }
  }

  for ( j = 0; j < 3; j++ )
  {
    for ( k = 0; k < FACE_MAX; k++ )
    {
      vertex_rgb[0][j][k] = 0.299;
      vertex_rgb[1][j][k] = 0.587;
      vertex_rgb[2][j][k] = 0.114;
    }
  }

  for ( iface = 0; iface < FACE_MAX; iface++ )
  {
    for ( ivert = 0; ivert < ORDER_MAX; ivert++ )
    {
      for ( i = 0; i < 2; i++ )
      {
        vertex_tex_uv[i][ivert][iface] = 0.0;
      }
    }
  }

  if ( debug )
  {
    cout << "\n";
    cout << "DATA_INIT: Graphics data initialized.\n";
  }

  return;
}
//****************************************************************************80

bool data_read ( )

//****************************************************************************80
//
//  Purpose:
//
//    DATA_READ reads a file into internal graphics data.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    08 September 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, bool DATA_READ, is TRUE if the data was read successfully.
//
{
  ifstream file_in;
  FILE *filein;
  char *filein_type;
  int icor3;
  int ierror;
  int iface;
  int iline;
  int ivert;
  int ntemp;
//
//  Retrieve the input file type.
//
  filein_type = file_ext ( filein_name );

  if ( filein_type == NULL )
  {
    cout << "\n";
    cout << "DATA_READ - Fatal error!\n";
    cout << "  Could not determine the type of '" << filein_name << "'.\n";
    return false;
  }
  else if ( debug )
  {
    cout << "\n";
    cout << "DATA_READ: Input file has type " << filein_type << ".\n";
  }
//
//  Initialize some data.
//
  max_order2 = 0;
  bad_num = 0;
  bytes_num = 0;
  comment_num = 0;
  dup_num = 0;
  text_num = 0;
//
//  Open the file.
//
  if ( s_eqi ( filein_type, "3DS" ) ||
       s_eqi ( filein_type, "STLB" ) ||
       s_eqi ( filein_type, "TRIB" ) )
  {
    filein = fopen ( filein_name, "rb" );

    if ( filein == NULL )
    {
      cout << "\n";
      cout << "DATA_READ - Fatal error!\n";
      cout << "  Could not open the input file '" << filein_name << "'!\n";
      return false;
    }
  }
  else if ( s_eqi ( filein_type, "OFF" ) )
  {
     file_in.open ( filein_name );

    if ( !file_in )
    {
      cout << "\n";
      cout << "DATA_READ - Fatal error!\n";
      cout << "  Could not open the input file '" << filein_name << "'!\n";
      return false;
    }
  }
  else
  {
    filein = fopen ( filein_name, "r" );

    if ( filein == NULL )
    {
      cout << "\n";
      cout << "DATA_READ - Fatal error!\n";
      cout << "  Could not open the input file '" << filein_name << "'!\n";
      return false;
    }
  }
//
//  Read the information in the file.
//
  if ( s_eqi ( filein_type, "3DS" ) )
  {
    ierror = tds_read ( filein );
//
//  Cleanup: distribute the node textures to the vertices.
//
    if ( ierror == 0 )
    {
      for ( iface = 0; iface < face_num; iface++ )
      {
        for ( ivert = 0; ivert < face_order[iface]; ivert++ )
        {
          icor3 = face[ivert][iface];
          vertex_tex_uv[0][ivert][iface] = cor3_tex_uv[0][icor3];
          vertex_tex_uv[1][ivert][iface] = cor3_tex_uv[1][icor3];
        }
      }

    }

  }
  else if ( s_eqi ( filein_type, "ASE" ) )
  {
    ierror = ase_read ( filein );

    if ( ierror == 0 )
    {
      node_to_vertex_material ( );
      vertex_to_face_material ( );
    }

  }
  else if ( s_eqi ( filein_type, "BYU" ) )
  {
    ierror = byu_read ( filein );
  }
  else if ( s_eqi ( filein_type, "DXF" ) )
  {
    ierror = dxf_read ( filein );
  }
  else if ( s_eqi ( filein_type, "GMOD" ) )
  {
    ierror = gmod_read ( filein );
  }
  else if ( s_eqi ( filein_type, "HRC" ) )
  {
    ierror = hrc_read ( filein );
  }
  else if ( s_eqi ( filein_type, "IV" ) )
  {
    ierror = iv_read ( filein );
  }
  else if ( s_eqi ( filein_type, "OBJ" ) )
  {
    ierror = obj_read ( filein );
  }
  else if ( s_eqi ( filein_type, "OFF" ) )
  {
    ierror = off_read ( file_in );
  }
  else if ( s_eqi ( filein_type, "SMF" ) )
  {
    ierror = smf_read ( filein );
  }
  else if (
    s_eqi ( filein_type, "STL" ) ||
    s_eqi ( filein_type, "STLA") )
  {
    ierror = stla_read ( filein );
  }
  else if ( s_eqi ( filein_type, "STLB") )
  {
    ierror = stlb_read ( filein );
  }
  else if (
    s_eqi ( filein_type, "TRI" ) ||
    s_eqi ( filein_type, "TRIA") )
  {
    ierror = tria_read ( filein );
  }
  else if ( s_eqi ( filein_type, "TRIB") )
  {
    ierror = trib_read ( filein );
  }
  else if ( s_eqi ( filein_type, "VLA" ) )
  {
    ierror = vla_read ( filein );
  }
  else
  {
    cout << "\n";
    cout << "DATA_READ - Fatal error!\n";
    cout << "  Unacceptable input file type.\n";
    return false;
  }

  if ( s_eqi ( filein_type, "OFF" ) )
  {
    file_in.close ( );
  }
  else
  {
    fclose ( filein );
  }

  if ( debug )
  {
    cout << "DATA_READ: Finished reading the data file.\n";
  }
//
//  Catch errors reported by the various reading routines.
//
  if ( ierror == ERROR )
  {
    cout << "\n";
    cout << "DATA_READ - Fatal error!\n";
    cout << "  There was an error reported while reading input data.\n";
    return false;
  }
//
//  Restore the transformation matrix.
//
  tmat_init ( transform_matrix );
//
//  Report on what we read.
//
  if ( face_num < FACE_MAX )
  {
    ntemp = face_num;
  }
  else
  {
    ntemp = FACE_MAX;
  }

  max_order2 = i4vec_max ( ntemp, face_order );

  data_report ( );
//
//  Warn about any errors that occurred during reading.
//
  if ( ierror == ERROR )
  {
    cout << "\n";
    cout << "DATA_READ - Fatal error!\n";
    cout << "  An error occurred while reading the input file.\n";
    return false;
  }
//
//  Check the data.
//  You MUST wait until after this check before doing other computations,
//  since COR3_NUM and other variables could be much larger than the legal
//  maximums, until corrected by DATA_CHECK.
//
  data_check ( );
//
//  MATERIALS FIXUPS:
//
//  If there are no materials at all, define one.
//
  if ( material_num < 1 )
  {
    material_num = 1;
    strcpy ( material_name[0], "Material_0000" );
    material_rgba[0][0] = 0.7;
    material_rgba[1][0] = 0.7;
    material_rgba[2][0] = 0.7;
    material_rgba[3][0] = 1.0;
  }
//
//  If a node has not been assigned a material, set it to material 0.
//
  for ( icor3 = 0; icor3 < cor3_num; icor3++ )
  {
    if ( cor3_material[icor3] < 0 || material_num - 1 < cor3_material[icor3] )
    {
      cor3_material[icor3] = 0;
    }
  }
//
//  If a vertex has not been assigned a material, set it to material 0.
//
  for ( iface = 0; iface < face_num; iface++ )
  {
    for ( ivert = 0; ivert < face_order[iface]; ivert++ )
    {
      if ( vertex_material[ivert][iface] < 0 ||
           material_num - 1 < vertex_material[ivert][iface] )
      {
        vertex_material[ivert][iface] = 0;
      }
    }
  }
//
//  If a face has not been assigned a material, set it to material 0.
//
  for ( iface = 0; iface < face_num; iface++ )
  {
    if ( face_material[iface] < 0 || material_num - 1 < face_material[iface] )
    {
      face_material[iface] = 0;
    }
  }
//
//  If a line item has not been assigned a material, set it to material 0.
//
  for ( iline = 0; iline < line_num; iline++ )
  {
    if ( line_dex[iline] == -1 )
    {
      line_material[iline] = -1;
    }
    else if ( line_material[iline] < 0 ||
             material_num - 1 < line_material[iline] )
    {
      line_material[iline] = 0;
    }
  }
//
//  Delete edges of zero length.
//
  edge_null_delete ( );
//
//  Compute the area of each face.
//
  face_area_set ( );
//
//  Delete faces with zero area.
//
  face_null_delete ( );
//
//  Recompute zero face-vertex normals from vertex positions.
//
  vertex_normal_set ( );
//
//  Compute the node normals from the vertex normals.
//
  cor3_normal_set ( );
//
//  Recompute zero face normals by averaging face-vertex normals.
//
  face_normal_ave ( );
//
//  Report on the nodal coordinate range.
//
  cor3_range ( );

  return true;
}
//****************************************************************************80

void data_report ( )

//****************************************************************************80
//
//  Purpose:
//
//    DATA_REPORT gives a summary of the contents of the data file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    24 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  cout << "\n";
  cout << "DATA_REPORT - The input file contains:\n";
  cout << "\n";
  cout << "  Bad data items             " << bad_num << "\n";
  cout << "  Text lines                 " << text_num << "\n";
  cout << "  Text bytes (binary data)   " << bytes_num << "\n";
  cout << "  Colors                     " << color_num << "\n";
  cout << "  Comments                   " << comment_num << "\n";
  cout << "  Duplicate points           " << dup_num << "\n";
  cout << "  Faces                      " << face_num << "\n";
  cout << "  Groups                     " << group_num << "\n";
  cout << "  Vertices per face, maximum " << max_order2 << "\n";
  cout << "  Line items                 " << line_num << "\n";
  cout << "  Points                     " << cor3_num << "\n";
  cout << "  Objects                    " << object_num << "\n";

  return;
}
//****************************************************************************80

int data_write ( )

//****************************************************************************80
//
//  Purpose:
//
//    DATA_WRITE writes the internal graphics data to a file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    22 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  FILE *fileout;
  char *fileout_type;
  int   line_num_save;
  int   result;

  result = 0;
//
//  Retrieve the output file type.
//
  fileout_type = file_ext ( fileout_name );

  if ( fileout_type == NULL )
  {
    cout << "\n";
    cout << "DATA_WRITE - Fatal error!\n";
    cout << "  Could not determine the output file type.\n";
    return 1;
  }
//
//  Open the output file.
//
  if ( s_eqi ( fileout_type, "3DS" ) ||
       s_eqi ( fileout_type, "STLB" ) ||
       s_eqi ( fileout_type, "TRIB" ) )
  {
    fileout = fopen ( fileout_name, "wb" );
  }
  else
  {
    fileout = fopen ( fileout_name, "w" );
  }

  if ( fileout == NULL )
  {
    cout << "\n";
    cout << "DATA_WRITE - Fatal error!\n";
    cout << "  Could not open the output file!\n";
    return 1;
  }
//
//  Write the output file.
//
  if ( s_eqi ( fileout_type, "3DS" ) )
  {
    tds_pre_process();
    result = tds_write ( fileout );
  }
  else if ( s_eqi ( fileout_type, "ASE" ) )
  {
    result = ase_write ( fileout );
  }
  else if ( s_eqi ( fileout_type, "BYU" ) )
  {
    result = byu_write ( fileout );
  }
  else if ( s_eqi ( fileout_type, "DXF" ) )
  {
    result = dxf_write ( fileout );
  }
  else if ( s_eqi ( fileout_type, "GMOD" ) )
  {
    result = gmod_write ( fileout );
  }
  else if ( s_eqi ( fileout_type, "HRC" ) )
  {
    result = hrc_write ( fileout );
  }
  else if ( s_eqi ( fileout_type, "IV" ) )
  {
    result = iv_write ( fileout );
  }
  else if ( s_eqi ( fileout_type, "OBJ" ) )
  {
    result = obj_write ( fileout );
  }
  else if ( s_eqi ( fileout_type, "OFF" ) )
  {
    result = off_write ( fileout );
  }
  else if ( s_eqi ( fileout_type, "POV" ) )
  {
    result = pov_write ( fileout );
  }
  else if ( s_eqi ( fileout_type, "SMF" ) )
  {
    result = smf_write ( fileout );
  }
  else if (
    s_eqi ( fileout_type, "STL" ) ||
    s_eqi ( fileout_type, "STLA" ) )
  {
    result = stla_write ( fileout );
  }
  else if ( s_eqi ( fileout_type, "STLB" ) )
  {
    result = stlb_write ( fileout );
  }
  else if ( s_eqi ( fileout_type, "TEC" ) )
  {
    result = tec_write ( fileout );
  }
  else if (
    s_eqi ( fileout_type, "TRI" ) ||
    s_eqi ( fileout_type, "TRIA" ) )
  {
    result = tria_write ( fileout );
  }
  else if ( s_eqi ( fileout_type, "TRIB" ) )
  {
    result = trib_write ( fileout );
  }
  else if ( s_eqi ( fileout_type, "TXT" ) )
  {
    result = txt_write ( fileout );
  }
  else if ( s_eqi ( fileout_type, "UCD" ) )
  {
    result = ucd_write ( fileout );
  }
  else if ( s_eqi ( fileout_type, "VLA" ) )
  {
    line_num_save = line_num;

    if ( 0 < face_num )
    {
      cout << "\n";
      cout << "DATA_WRITE - Note:\n";
      cout << "  Face information will temporarily be converted to\n";
      cout << "  line information for output to a VLA file.\n";

      face_to_line ( );

      if ( LINES_MAX < line_num )
      {
        cout << "\n";
        cout << "DATA_WRITE - Warning:\n";
        cout << "  Some face information was lost.\n";
        cout << "  The maximum number of lines is " << LINES_MAX << ".\n";
        cout << "  The number of lines needed is " << line_num << ".\n";
        line_num = LINES_MAX;
      }

    }

    result = vla_write ( fileout );

    line_num = line_num_save;

  }
  else if ( s_eqi ( fileout_type, "WRL" ) )
  {
    result = wrl_write ( fileout );
  }
  else if ( s_eqi ( fileout_type, "XGL" ) )
  {
    result = xgl_write ( fileout );
  }
  else
  {
    result = ERROR;
    cout << "\n";
    cout << "DATA_WRITE - Fatal error!\n";
    cout << "  Unacceptable output file type '" << fileout_type << "'.\n";
  }
//
//  Close the output file.
//
  fclose ( fileout );

  if ( result == ERROR )
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
//****************************************************************************80

int dxf_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    DXF_READ reads an AutoCAD DXF file.
//
//  Example:
//
//      0
//    SECTION
//      2
//    HEADER
//    999
//    diamond.dxf created by IVREAD.
//    999
//    Original data in diamond.obj.
//      0
//    ENDSEC
//      0
//    SECTION
//      2
//    TABLES
//      0
//    ENDSEC
//      0
//    SECTION
//      2
//    BLOCKS
//      0
//    ENDSEC
//      0
//    SECTION
//      2
//    ENTITIES
//      0
//    LINE
//      8
//    0
//     10
//      0.00  (X coordinate of beginning of line.)
//     20
//      0.00  (Y coordinate of beginning of line.)
//     30
//      0.00  (Z coordinate of beginning of line.)
//     11
//      1.32  (X coordinate of end of line.)
//     21
//      1.73  (Y coordinate of end of line.)
//     31
//      2.25  (Z coordinate of end of line.)
//      0
//    3DFACE
//      8
//     Cube
//    10
//    -0.50  (X coordinate of vertex 1)
//    20
//     0.50  (Y coordinate of vertex 1)
//    30
//      1.0  (Z coordinate of vertex 1)
//    11
//     0.50  (X coordinate of vertex 2)
//    21
//     0.50  (Y coordinate of vertex 2)
//    31
//      1.0  (Z coordinate of vertex 2)
//    12
//     0.50  (X coordinate of vertex 3)
//    22
//     0.50  (Y coordinate of vertex 3)
//    32
//     0.00  (Z coordinate of vertex 3)
//      0
//    ENDSEC
//      0
//    EOF
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    23 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  int   code;
  int   count;
  float cvec[3];
  int   icor3;
  char  input1[LINE_MAX_LEN];
  char  input2[LINE_MAX_LEN];
  int   ivert;
  float rval;
  int   width;
  int   linemode;
  int   cpos;

  linemode = 0;
  ivert = 0;
//
//  Read the next two lines of the file into INPUT1 and INPUT2.
//

  for ( ;; )
  {

//
//  INPUT1 should contain a single integer, which tells what INPUT2
//  will contain.
//
    if ( fgets ( input1, LINE_MAX_LEN, filein ) == NULL )
    {
      break;
    }

    text_num = text_num + 1;

    count = sscanf ( input1, "%d%n", &code, &width );
    if ( count <= 0 )
    {
      break;
    }
//
//  Read the second line, and interpret it according to the code.
//
    if ( fgets ( input2, LINE_MAX_LEN, filein ) == NULL )
    {
      break;
    }

    text_num = text_num + 1;

    if ( code == 0 )
    {

//
//  Finish off the face.
//
      if ( 0 < ivert )
      {
        face_order[face_num] = ivert;
        face_num = face_num + 1;
        ivert = 0;
      }

      if ( strncmp( input2, "LINE", 4 ) == 0 )
      {
        linemode = 1;
      }
      else if ( strncmp( input2, "3DFACE", 6 ) == 0 )
      {
        linemode = 0;
        ivert = 0;
      }
    }
    else
    {
      for (cpos = 0; input1[cpos] == ' '; cpos++)
      { };

      if ( input1[cpos] == '1' || input1[cpos] == '2' || input1[cpos] == '3' )
      {
        count = sscanf ( input2, "%e%n", &rval, &width );

        switch ( input1[cpos] )
        {
          case '1':
            if ( 0 < line_num )
            {
              if ( linemode )
              {
                line_dex[line_num] = - 1;
                line_material[line_num] = - 1;
                line_num = line_num + 1;
              }
            }
            cvec[0] = rval;
            break;

          case '2':
            cvec[1] = rval;
            break;

          case '3':
            cvec[2] = rval;

            if ( cor3_num < 1000 )
            {
              icor3 = rcol_find ( cor3, 3, cor3_num, cvec );
            }
            else
            {
              icor3 = -1;
            }

            if ( icor3 == -1 )
            {
              icor3 = cor3_num;
              if ( cor3_num < COR3_MAX )
              {
                cor3[0][cor3_num] = cvec[0];
                cor3[1][cor3_num] = cvec[1];
                cor3[2][cor3_num] = cvec[2];
              }
              cor3_num = cor3_num + 1;
            }
            else
            {
              dup_num = dup_num + 1;
            }

            if ( linemode )
            {
              line_dex[line_num] = icor3;
              line_material[line_num] = 0;
              line_num = line_num + 1;
            }
            else
            {
              face[ivert][face_num] = icor3;
              ivert = ivert + 1;
            }
            break;

          default:
            break;
        }
      }
    }
  }

  if ( 0 < line_num )
  {
    if ( linemode )
    {
      line_dex[line_num] = - 1;
      line_material[line_num] = - 1;
      line_num = line_num + 1;
    }
  }
  return 0;
}
//****************************************************************************80

int dxf_write ( FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    DXF_WRITE writes graphics information to an AutoCAD DXF file.
//
//  Example:
//
//      0
//    SECTION
//      2
//    HEADER
//    999
//    diamond.dxf created by IVREAD.
//    999
//    Original data in diamond.obj.
//      0
//    ENDSEC
//      0
//    SECTION
//      2
//    TABLES
//      0
//    ENDSEC
//      0
//    SECTION
//      2
//    BLOCKS
//      0
//    ENDSEC
//      0
//    SECTION
//      2
//    ENTITIES
//      0
//    LINE
//      8
//    0
//     10
//      0.00  (X coordinate of beginning of line.)
//     20
//      0.00  (Y coordinate of beginning of line.)
//     30
//      0.00  (Z coordinate of beginning of line.)
//     11
//      1.32  (X coordinate of end of line.)
//     21
//      1.73  (Y coordinate of end of line.)
//     31
//      2.25  (Z coordinate of end of line.)
//      0
//    3DFACE
//      8
//     Cube
//    10
//    -0.50  (X coordinate of vertex 1)
//    20
//     0.50  (Y coordinate of vertex 1)
//    30
//      1.0  (Z coordinate of vertex 1)
//    11
//     0.50  (X coordinate of vertex 2)
//    21
//     0.50  (Y coordinate of vertex 2)
//    31
//      1.0  (Z coordinate of vertex 2)
//    12
//     0.50  (X coordinate of vertex 3)
//    22
//     0.50  (Y coordinate of vertex 3)
//    32
//     0.00  (Z coordinate of vertex 3)
//      0
//    ENDSEC
//      0
//    EOF
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    16 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  int icor3;
  int iline;
  int iface;
  int ivert;
  int jcor3;
  bool newline;
  int text_num;
//
//  Initialize.
//
  text_num = 0;

  fprintf ( fileout, "  0\n" );
  fprintf ( fileout, "SECTION\n" );
  fprintf ( fileout, "  2\n" );
  fprintf ( fileout, "HEADER\n" );
  fprintf ( fileout, "999\n" );
  fprintf ( fileout, "%s created by IVCON.\n", fileout_name );
  fprintf ( fileout, "999\n" );
  fprintf ( fileout, "Original data in %s.\n", filein_name );
  fprintf ( fileout, "  0\n" );
  fprintf ( fileout, "ENDSEC\n" );
  text_num = text_num + 10;

  fprintf ( fileout, "  0\n" );
  fprintf ( fileout, "SECTION\n" );
  fprintf ( fileout, "  2\n" );
  fprintf ( fileout, "TABLES\n" );
  fprintf ( fileout, "  0\n" );
  fprintf ( fileout, "ENDSEC\n" );
  text_num = text_num + 6;

  fprintf ( fileout, "  0\n" );
  fprintf ( fileout, "SECTION\n" );
  fprintf ( fileout, "  2\n" );
  fprintf ( fileout, "BLOCKS\n" );
  fprintf ( fileout, "  0\n" );
  fprintf ( fileout, "ENDSEC\n" );
  text_num = text_num + 6;

  fprintf ( fileout, "  0\n" );
  fprintf ( fileout, "SECTION\n" );
  fprintf ( fileout, "  2\n" );
  fprintf ( fileout, "ENTITIES\n" );
  text_num = text_num + 4;
//
//  Handle lines.
//
  jcor3 = 0;
  newline = true;

  for ( iline = 0; iline < line_num; iline++ )
  {
    icor3 = line_dex[iline];

    if ( icor3 == -1 )
    {
      newline = true;
    }
    else
    {
      if ( !newline )
      {
        fprintf ( fileout, "  0\n" );
        fprintf ( fileout, "LINE\n" );
        fprintf ( fileout, "  8\n" );
        fprintf ( fileout, "  0\n" );
        fprintf ( fileout, " 10\n" );
        fprintf ( fileout, "%f\n", cor3[0][jcor3] );
        fprintf ( fileout, " 20\n" );
        fprintf ( fileout, "%f\n", cor3[1][jcor3] );
        fprintf ( fileout, " 30\n" );
        fprintf ( fileout, "%f\n", cor3[2][jcor3] );
        fprintf ( fileout, " 11\n" );
        fprintf ( fileout, "%f\n", cor3[0][icor3] );
        fprintf ( fileout, " 21\n" );
        fprintf ( fileout, "%f\n", cor3[1][icor3] );
        fprintf ( fileout, " 31\n" );
        fprintf ( fileout, "%f\n", cor3[2][icor3] );

        text_num = text_num + 16;
      }
      jcor3 = icor3;
      newline = false;
    }
  }
//
//  Handle faces.
//  (If FACE_ORDER is greater than 10, you're sure to have problems here)
//
  for ( iface = 0; iface < face_num; iface++ )
  {
    fprintf ( fileout, "  0\n" );
    fprintf ( fileout, "3DFACE\n" );
    fprintf ( fileout, "  8\n" );
    fprintf ( fileout, "  Cube\n" );
    text_num = text_num + 4;

    for ( ivert = 0; ivert < face_order[iface]; ivert++ )
    {
      icor3 = face[ivert][iface];

      fprintf ( fileout, "1%d\n", ivert );
      fprintf ( fileout, "%f\n", cor3[0][icor3] );
      fprintf ( fileout, "2%d\n", ivert );
      fprintf ( fileout, "%f\n", cor3[1][icor3] );
      fprintf ( fileout, "3%d\n", ivert );
      fprintf ( fileout, "%f\n", cor3[2][icor3] );

      text_num = text_num + 6;
    }
  }

  fprintf ( fileout, "  0\n" );
  fprintf ( fileout, "ENDSEC\n" );
  fprintf ( fileout, "  0\n" );
  fprintf ( fileout, "EOF\n" );
  text_num = text_num + 4;
//
//  Report.
//
  cout << "\n";
  cout << "DXF_WRITE - Wrote " << text_num << " text lines.\n";

  return 0;
}
//****************************************************************************80

int edge_count ( )

//****************************************************************************80
//
//  Purpose:
//
//    EDGE_COUNT determines the number of edges in a graph.
//
//  Discussion:
//
//    The routine extracts the successive pairs of vertices that
//    define each edge of a face.  It reorders each pair so that
//    the lesser element is listed first.  It sorts the entire list.
//    Then it counts the unique entries.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    08 September 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, int EDGE_COUNT, the number of unique edges.
//
{
  int *edge;
  int edge_num;
  int edge_num_old;
  int i;
  int iface;
  int indx;
  int isgn;
  int j;
  int k;
  int vert;
  int vert2;
//
//  First count the number of edges with duplication.
//
  edge_num = 0;
  for ( iface = 0; iface < face_num; iface++ )
  {
    edge_num = edge_num + face_order[iface];
  }
//
//  Allocate space, and store the edges.
//
  edge = new int[edge_num*2];

  edge_num = 0;
  for ( iface = 0; iface < face_num; iface++ )
  {
    for ( vert = 0; vert < face_order[iface]; vert++ )
    {
      i = face[vert][iface];
      vert2 = i4_wrap ( vert+1, 0, face_order[iface]-1 );
      j = face[vert2][iface];
      edge[0+edge_num*2] = i4_min ( i, j );
      edge[1+edge_num*2] = i4_max ( i, j );
      edge_num = edge_num + 1;
    }
  }
//
//  Sort the edges.
//
  i = 0;
  indx = 0;
  isgn = 0;
  j = 0;
//
//  Call the external heap sorter.
//
  for ( ; ; )
  {
    sort_heap_external ( edge_num, &indx, &i, &j, isgn );
//
//  Interchange the I and J objects.
//
    if ( 0 < indx )
    {
      k           = edge[0+i*2];
      edge[0+i*2] = edge[0+j*2];
      edge[0+j*2] = k;
      k           = edge[1+i*2];
      edge[1+i*2] = edge[1+j*2];
      edge[1+j*2] = k;
    }
//
//  Compare the I and J objects.
//
    else if ( indx < 0 )
    {
      if ( edge[0+i*2] < edge[0+j*2] )
      {
        isgn = -1;
      }
      else if ( edge[0+i*2] == edge[0+j*2] )
      {
        if ( edge[1+i*2] < edge[1+j*2] )
        {
          isgn = -1;
        }
        else if ( edge[1+i*2] == edge[1+j*2] )
        {
          isgn = 0;
        }
        else
        {
          isgn = 1;
        }
      }
      else
      {
        isgn = 1;
      }
    }
    else if ( indx == 0 )
    {
      break;
    }

  }
//
//  Count the unique entries.
//
  edge_num_old = edge_num;

  edge_num = 0;

  for ( i = 0; i < edge_num_old; i++ )
  {
    if ( i == 0 )
    {
      edge_num = 1;
    }
    else
    {
      if ( edge[0+(i-1)*2] != edge[0+i*2] ||
          edge[1+(i-1)*2] != edge[1+i*2] )
      {
        edge_num = edge_num + 1;
      }
    }

  }

  delete [] edge;

  return edge_num;
}
//****************************************************************************80

void edge_null_delete ( )

//****************************************************************************80
//
//  Purpose:
//
//    EDGE_NULL_DELETE deletes face edges with zero length.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    16 July 1999
//
//  Author:
//
//    John Burkardt
//
{
  float distsq;
  int face2[ORDER_MAX];
  int face_order2;
  int iface;
  int inode;
  int ivert;
  int j;
  int jnode;
  int jvert;
  int edge_num;
  int edge_num_del;
  float vertex_normal2[3][ORDER_MAX];
  float x;
  float y;
  float z;

  edge_num = 0;
  edge_num_del = 0;
//
//  Consider each face.
//
  for ( iface = 0; iface < face_num; iface++ )
  {
//
//  Consider each pair of consecutive vertices.
//
    face_order2 = 0;

    for ( ivert = 0; ivert < face_order[iface]; ivert++ )
    {
      edge_num = edge_num + 1;

      jvert = ivert + 1;
      if ( face_order[iface] <= jvert )
      {
        jvert = 0;
      }

      inode = face[ivert][iface];
      jnode = face[jvert][iface];


      x = cor3[0][inode] - cor3[0][jnode];
      y = cor3[1][inode] - cor3[1][jnode];
      z = cor3[2][inode] - cor3[2][jnode];

      distsq = x * x + y * y + z * z;

      if ( distsq != 0.0 )
      {
        face2[face_order2] = face[ivert][iface];
        vertex_normal2[0][face_order2] = vertex_normal[0][ivert][iface];
        vertex_normal2[1][face_order2] = vertex_normal[1][ivert][iface];
        vertex_normal2[2][face_order2] = vertex_normal[2][ivert][iface];
        face_order2 = face_order2 + 1;
      }
      else
      {
        edge_num_del = edge_num_del + 1;
      }

    }

    face_order[iface] = face_order2;
    for ( ivert = 0; ivert < face_order[iface]; ivert++ )
    {
      face[ivert][iface] = face2[ivert];
      for ( j = 0; j < 3; j++ )
      {
        vertex_normal[j][ivert][iface] = vertex_normal2[j][ivert];
      }
    }

  }

  cout << "\n";
  cout << "EDGE_NULL_DELETE:\n";
  cout << "  There are a total of " << edge_num << " edges.\n";
  cout << "  Of these, " << edge_num_del << " were of zero length,\n";
  cout << "  and were deleted.\n";

  return;
}
//****************************************************************************80

void face_area_set ( )

//****************************************************************************80
//
//  Purpose:
//
//    FACE_AREA_SET computes the area of the faces.
//
//  Discussion:
//
//    The area is the sum of the areas of the triangles formed by
//    node N with consecutive pairs of nodes.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    17 July 1999
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Adrian Bowyer and John Woodwark,
//    A Programmer's Geometry,
//    Butterworths, 1983.
//
{
  float alpha;
  float area_max;
  float area_min;
  float area_tri;
  float base;
  float dot;
  float height;
  int i;
  int i1;
  int i2;
  int i3;
  int iface;
  int face_num_del;
  float tol;
  float x;
  float x1;
  float x2;
  float x3;
  float y;
  float y1;
  float y2;
  float y3;
  float z;
  float z1;
  float z2;
  float z3;

  for ( iface = 0; iface < face_num; iface++ )
  {
    face_area[iface] = 0.0;

    for ( i = 0; i < face_order[iface]-2; i++ )
    {
      i1 = face[i][iface];
      i2 = face[i+1][iface];
      i3 = face[i+2][iface];

      x1 = cor3[0][i1];
      y1 = cor3[1][i1];
      z1 = cor3[2][i1];

      x2 = cor3[0][i2];
      y2 = cor3[1][i2];
      z2 = cor3[2][i2];

      x3 = cor3[0][i3];
      y3 = cor3[1][i3];
      z3 = cor3[2][i3];
//
//  Find the projection of (P3-P1) onto (P2-P1).
//
      dot =
        ( x2 - x1 ) * ( x3 - x1 ) +
        ( y2 - y1 ) * ( y3 - y1 ) +
        ( z2 - z1 ) * ( z3 - z1 );

      base = sqrt (
          ( x2 - x1 ) * ( x2 - x1 )
        + ( y2 - y1 ) * ( y2 - y1 )
        + ( z2 - z1 ) * ( z2 - z1 ) );
//
//  The height of the triangle is the length of (P3-P1) after its
//  projection onto (P2-P1) has been subtracted.
//
      if ( base == 0.0 )
      {
        height = 0.0;
      }
      else
      {
        alpha = dot / ( base * base );

        x = x3 - x1 - alpha * ( x2 - x1 );
        y = y3 - y1 - alpha * ( y2 - y1 );
        z = z3 - z1 - alpha * ( z2 - z1 );

        height = sqrt ( x * x + y * y + z * z );
      }

      area_tri = 0.5 * base * height;
      face_area[iface] = face_area[iface] + area_tri;

    }

  }

  area_min = face_area[0];
  area_max = face_area[0];

  for ( iface = 1; iface < face_num; iface++ )
  {
    if ( face_area[iface] < area_min )
    {
      area_min = face_area[iface];
    }
    if ( area_max < face_area[iface] )
    {
      area_max = face_area[iface];
    }
  }

  cout << "\n";
  cout << "FACE_AREA_SET:\n";
  cout << "  Minimum face area is " << area_min << "\n";
  cout << "  Maximum face area is " << area_max << "\n";

  tol = area_max / 10000.0;

  if ( area_min < tol )
  {
    face_num_del = 0;

    for ( iface = 0; iface < face_num; iface++ )
    {
      if ( face_area[iface] < tol )
      {
        face_order[iface] = 0;
        face_num_del = face_num_del + 1;
      }
    }

    cout << "  Marked " << face_num_del << " tiny faces for deletion.\n";

  }

  return;
}
//****************************************************************************80

void face_normal_ave ( )

//****************************************************************************80
//
//  Purpose:
//
//    FACE_NORMAL_AVE sets face normals as average of face vertex normals.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    09 October 1998
//
//  Author:
//
//    John Burkardt
//
{
  int i;
  int iface;
  int ivert;
  int nfix;
  float norm;
  float x;
  float y;
  float z;

  if ( face_num <= 0 )
  {
    return;
  }

  nfix = 0;

  for ( iface = 0; iface < face_num; iface++ )
  {
//
//  Check the norm of the current normal vector.
//
    x = face_normal[0][iface];
    y = face_normal[1][iface];
    z = face_normal[2][iface];
    norm = ( float ) sqrt ( x * x + y * y + z * z );

    if ( norm == 0.0 )
    {
      nfix = nfix + 1;

      for ( i = 0; i < 3; i++ )
      {
        face_normal[i][iface] = 0.0;
      }

      for ( ivert = 0; ivert < face_order[iface]; ivert++ )
      {
        for ( i = 0; i < 3; i++ )
        {
          face_normal[i][iface] = face_normal[i][iface] +
            vertex_normal[i][ivert][iface];
        }
      }

      x = face_normal[0][iface];
      y = face_normal[1][iface];
      z = face_normal[2][iface];
      norm = ( float ) sqrt ( x * x + y * y + z * z );

      if ( norm == 0.0 )
      {
        for ( i = 0; i < 3; i++ )
        {
          face_normal[i][iface] = ( float ) ( 1.0 / sqrt ( 3.0 ) );
        }
      }
      else
      {
        for ( i = 0; i < 3; i++ )
        {
          face_normal[i][iface] = face_normal[i][iface] / norm;
        }
      }
    }
  }

  if ( 0 < nfix )
  {
    cout << "\n";
    cout << "FACE_NORMAL_AVE: Recomputed " << nfix << " face normals\n";
    cout << "  by averaging face vertex normals.\n";
  }
  return;
}
//****************************************************************************80

void face_null_delete ( )

//****************************************************************************80
//
//  Purpose:
//
//    FACE_NULL_DELETE deletes faces of order less than 3.
//
//  Discussion:
//
//    Thanks to Susan M. Fisher, University of North Carolina,
//    Department of Computer Science, for pointing out a coding error
//    in FACE_NULL_DELETE that was overwriting all the data!
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    30 November 1999
//
//  Author:
//
//    John Burkardt
//
{
  int iface;
  int ivert;
  int j;
  int face_num2;
//
//  FACE_NUM2 is the number of faces we'll keep.
//
  face_num2 = 0;
//
//  Check every face.
//
  for ( iface = 0; iface < face_num; iface++ )
  {
//
//  Keep it only if it has order 3 or more.
//
    if ( 3 <= face_order[iface] )
    {
//
//  We don't have to slide data down in the array until
//  NUMFACE2 and IFACE get out of synch, that is, after
//  we've discarded at least one face.
//
      if ( face_num2 != iface )
      {
        face_area[face_num2] = face_area[iface];
        face_material[face_num2] = face_material[iface];
        face_order[face_num2] = face_order[iface];
        for ( ivert = 0; ivert < ORDER_MAX; ivert++ )
        {
          face[ivert][face_num2] = face[ivert][iface];
          vertex_material[ivert][face_num2] = vertex_material[ivert][iface];
          for ( j = 0; j < 3; j++ )
          {
            vertex_normal[j][ivert][face_num2] = vertex_normal[j][ivert][iface];
          }
        }

      }
//
//  Update the count only after we've used the un-incremented value
//  as a pointer.
//
      face_num2 = face_num2 + 1;

    }

  }

  cout << "\n";
  cout << "FACE_NULL_DELETE\n";
  cout << "  There are a total of " << face_num << " faces.\n";
  cout << "  Of these, " << face_num2 << " passed the order test.\n";

  face_num = face_num2;

  return;
}
//****************************************************************************80

int face_print ( int iface )

//****************************************************************************80
//
//  Purpose:
//
//    FACE_PRINT prints out information about a face.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    31 August 1998
//
//  Author:
//
//    John Burkardt
//
{
  int ivert;
  int j;
  int k;

  if ( iface < 0 || face_num-1 < iface )
  {
    cout << "\n";
    cout << "FACE_PRINT - Fatal error!\n";
    cout << "  Face indices must be between 1 and " << face_num << "\n";
    cout << "  But your requested value was " << iface << "\n";
    return 1;
  }

  cout << "\n";
  cout << "FACE_PRINT\n";
  cout << "  Information about face " << iface << "\n";
  cout << "\n";
  cout << "  Number of vertices is " << face_order[iface] << "\n";
  cout << "\n";
  cout << "  Vertex list:\n";
  cout << "    Vertex #, Node #, Material #, X, Y, Z:\n";
  cout << "\n";
  for ( ivert = 0; ivert < face_order[iface]; ivert++ )
  {
    j = face[ivert][iface];
    k = vertex_material[ivert][iface];
    cout
     << setw(6)  << ivert      << "  "
     << setw(6)  << j          << "  "
     << setw(6)  << k          << "  "
     << setw(10) << cor3[0][j] << "  "
     << setw(10) << cor3[1][j] << "  "
     << setw(10) << cor3[2][j] << "\n";
  }

  cout << "\n";
  cout << "  Face normal vector:\n";
  cout << "\n";
  cout
    << setw(10) << face_normal[0][iface] << "  "
    << setw(10) << face_normal[1][iface] << "  "
    << setw(10) << face_normal[2][iface] << "\n";

  cout << "\n";
  cout << "  Vertex face normals:\n";;
  cout << "\n";
  for ( ivert = 0; ivert < face_order[iface]; ivert++ )
  {
    cout
      << setw(6)  << ivert                          << "  "
      << setw(10) << vertex_normal[0][ivert][iface] << "  "
      << setw(10) << vertex_normal[1][ivert][iface] << "  "
      << setw(10) << vertex_normal[2][ivert][iface] << "\n";
  }

  return 0;

}
//****************************************************************************80

void face_reverse_order ( )

//****************************************************************************80
//
//  Purpose:
//
//    FACE_REVERSE_ORDER reverses the order of the nodes in each face.
//
//  Discussion:
//
//    Reversing the order of the nodes requires that the normal vectors
//    be reversed as well, so this routine will automatically reverse
//    the normals associated with nodes, vertices and faces.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    28 June 1999
//
//  Author:
//
//    John Burkardt
//
{
  int i;
  int iface;
  int itemp;
  int ivert;
  int j;
  int m;
  float temp;

  for ( iface = 0; iface < face_num; iface++ )
  {
    m = face_order[iface];

    for ( ivert = 0; ivert < ( m / 2 ); ivert++ )
    {
      itemp = face[ivert][iface];
      face[ivert][iface] = face[m-1-ivert][iface];
      face[m-1-ivert][iface] = itemp;

      itemp = vertex_material[ivert][iface];
      vertex_material[ivert][iface] = vertex_material[m-1-ivert][iface];
      vertex_material[m-1-ivert][iface] = itemp;

      for ( j = 0; j < 3; j++ )
      {
        temp = vertex_normal[j][ivert][iface];
        vertex_normal[j][ivert][iface] = vertex_normal[j][m-1-ivert][iface];
        vertex_normal[j][m-1-ivert][iface] = temp;
      }

      for ( j = 0; j < 2; j++ )
      {
        temp = vertex_tex_uv[j][ivert][iface];
        vertex_tex_uv[j][ivert][iface] = vertex_tex_uv[j][m-1-ivert][iface];
        vertex_tex_uv[j][m-1-ivert][iface] = temp;
      }

    }

  }

  for ( i = 0; i < cor3_num; i++ )
  {
    for ( j = 0; j < 3; j++ )
    {
      cor3_normal[j][i] = - cor3_normal[j][i];
    }
  }

  for ( i = 0; i < face_num; i++ )
  {
    for ( j = 0; j < 3; j++ )
    {
      face_normal[j][i] = - face_normal[j][i];
    }
  }

  cout << "\n";
  cout << "FACE_REVERSE_ORDER\n";
  cout << "  Each list of nodes defining a face\n";
  cout << "  has been reversed; related information,\n";
  cout << "  including normal vectors, was also updated.\n";

  return;
}
//****************************************************************************80

int face_subset ( )

//****************************************************************************80
//
//  Purpose:
//
//    FACE_SUBSET selects a subset of the current faces as the new object.
//
//  Discussion:
//
//    The original graphic object is overwritten by the new one.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    12 October 1998
//
//  Author:
//
//    John Burkardt
//
//
{
  int i;
  int iface;
  int iface1;
  int iface2;
  int inc;
  int ivert;
  int j;
  int k;
  int cor3_num2;

  line_num = 0;
//
//  Get the first and last faces to save, IFACE1 and IFACE2.
//
  cout << "\n";
  cout << "Enter lowest face number to save between 0 and "
       << face_num - 1 << ":  ";
  scanf ( "%d", &iface1 );
  if ( iface1 < 0 || face_num - 1 < iface1 )
  {
    cout << "Illegal choice!\n";
    return 1;
  }

  cout << "\n";
  cout << "Enter highest face number to save between "
    << iface1 << " and " << face_num - 1 << ":  ";
  scanf ( "%d", &iface2 );
  if ( iface2 < iface1 || face_num - 1 < iface2 )
  {
    cout << "Illegal choice!\n";
    return 1;
  }

  inc = iface1;
//
//  "Slide" the data for the saved faces down the face arrays.
//
  for ( iface = 0; iface < iface2 + 1 - iface1; iface++ )
  {
    face_order[iface] = face_order[iface+inc];
    for ( ivert = 0; ivert < ORDER_MAX; ivert++ )
    {
      face[ivert][iface] = face[ivert][iface+inc];
      vertex_material[ivert][iface] = vertex_material[ivert][iface+inc];
      for ( i = 0; i < 3; i++ )
      {
        vertex_normal[i][ivert][iface] =
          vertex_normal[i][ivert][iface+inc];
        vertex_rgb[i][ivert][iface] = vertex_rgb[i][ivert][iface+inc];
      }
    }
    for ( i = 0; i < 3; i++ )
    {
      face_normal[i][iface] = face_normal[i][iface+inc];
    }
  }
//
//  Now reset the number of faces.
//
  face_num = iface2 + 1 - iface1;
//
//  Now, for each point I, set LIST(I) = J if point I is the J-th
//  point we are going to save, and 0 otherwise.  Then J will be
//  the new label of point I.
//
  for ( i = 0; i < cor3_num; i++ )
  {
    list[i] = -1;
  }

  cor3_num2 = 0;

  for ( iface = 0; iface < face_num; iface++ )
  {
    for ( ivert = 0; ivert < face_order[iface]; ivert++ )
    {
      j = face[ivert][iface];
      if ( list[j] == -1 )
      {
        cor3_num2 = cor3_num2 + 1;
        list[j] = cor3_num2;
      }
    }
  }
//
//  Now make the nonzero list entries rise in order, so that
//  we can compress the COR3 data in a minute.
//
  cor3_num2 = 0;

  for ( i = 0; i < cor3_num; i++ )
  {
    if ( list[i] != -1 )
    {
      list[i] = cor3_num2;
      cor3_num2 = cor3_num2 + 1;
    }
  }
//
//  Relabel the FACE array with the new node indices.
//
  for ( iface = 0; iface < face_num; iface++ )
  {
    for ( ivert = 0; ivert < face_order[iface]; ivert++ )
    {
      j = face[ivert][iface];
      face[ivert][iface] = list[j];
    }
  }
//
//  Rebuild the COR3 array by sliding data down.
//
  for ( i = 0; i < cor3_num; i++ )
  {
    k = list[i];
    if ( k != -1 )
    {
      for ( j = 0; j < 3; j++ )
      {
        cor3[j][k] = cor3[j][i];
      }
    }
  }

  cor3_num = cor3_num2;

  return 0;
}
//****************************************************************************80

void face_to_line ( )

//****************************************************************************80
//
//  Purpose:
//
//    FACE_TO_LINE converts face information to line information.
//
//  Discussion:
//
//    In some cases, the graphic information represented by polygonal faces
//    must be converted to a representation based solely on line segments.
//    This is particularly true if a VLA file is being written.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    26 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  int icor3;
  int iface;
  int ivert;
  int jcor3;
  int jvert;
//
//  Case 0:
//  No line pruning.
//
  if ( line_prune == 0 )
  {
    for ( iface = 0; iface < face_num; iface++ )
    {
      for ( ivert = 0; ivert < face_order[iface]; ivert++ )
      {
        icor3 = face[ivert][iface];

        line_num = line_num + 1;
        if ( line_num <= LINES_MAX )
        {
          line_dex[line_num] = icor3;
          line_material[line_num] = vertex_material[ivert][iface];
        }
      }

      ivert = 0;
      icor3 = face[ivert][iface];

      line_num = line_num + 1;
      if ( line_num <= LINES_MAX )
      {
        line_dex[line_num] = icor3;
        line_material[line_num] = vertex_material[ivert][iface];
      }

      line_num = line_num + 1;
      if ( line_num <= LINES_MAX )
      {
        line_dex[line_num] = -1;
        line_material[line_num] = -1;
      }
    }

  }
//
//  Case 2:
//  Simple-minded line pruning.
//  Only draw line (I,J) if I < J.
//
  else
  {
    for ( iface = 0; iface < face_num; iface++ )
    {
      for ( ivert = 0; ivert < face_order[iface]; ivert++ )
      {
        icor3 = face[ivert][iface];

        if ( ivert + 1 < face_order[iface] )
        {
          jvert = ivert + 1;
        }
        else
        {
          jvert = 0;
        }

        jcor3 = face[jvert][iface];

        if ( icor3 < jcor3 )
        {
          if ( line_num + 3 < LINES_MAX )
          {
            line_num = line_num + 1;
            line_dex[line_num] = icor3;
            line_material[line_num] = vertex_material[ivert][iface];

            line_num = line_num + 1;
            line_dex[line_num] = jcor3;
            line_material[line_num] = vertex_material[jvert][iface];

            line_num = line_num + 1;
            line_dex[line_num] = -1;
            line_material[line_num] = -1;

          }
        }
      }
    }

  }

  return;
}
//****************************************************************************80

void face_to_vertex_material ( )

//****************************************************************************80
//
//  Purpose:
//
//    FACE_TO_VERTEX_MAT extends face material definitions to vertices.
//
//  Discussion:
//
//    Assuming material indices are defined for all the faces, this
//    routine assigns to each vertex of a face the material of that face.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    22 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  int iface;
  int ivert;

  for ( iface = 0; iface < face_num; iface++ )
  {
    for ( ivert = 0; ivert < face_order[iface]; ivert++ )
    {
      vertex_material[ivert][iface] = face_material[iface];
    }
  }

  return;
}
//****************************************************************************80

char *file_ext ( char *file_name )

//****************************************************************************80
//
//  Purpose:
//
//    FILE_EXT picks out the extension in a file name.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    21 July 1998
//
//  Author:
//
//    John Burkardt
//
{
  int i;

  i = ch_index_last ( file_name, '.' );

  if ( i == -1 )
  {
    return NULL;
  }
  else
  {
    return file_name + i + 1;
  }
}
//****************************************************************************80

float float_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    FLOAT_READ reads 1 float from a binary file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    24 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  float rval;
  float temp;

  fread ( &temp, sizeof ( float ), 1, filein );

  if ( byte_swap )
  {
    rval = float_reverse_bytes ( temp );
  }
  else
  {
    rval = temp;
  }

  return rval;
}
//****************************************************************************80

float float_reverse_bytes ( float x )

//****************************************************************************80
//
//  Purpose:
//
//    FLOAT_REVERSE_BYTES reverses the four bytes in a float.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    24 May 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    X, a float whose bytes are to be reversed.
//
//    FLOAT_REVERSE_BYTES, a float with bytes in reverse order from those in X.
//
{
  char c;
  union {
    float yfloat;
    char ychar[4];
  } y;

  y.yfloat = x;

  c = y.ychar[0];
  y.ychar[0] = y.ychar[3];
  y.ychar[3] = c;

  c = y.ychar[1];
  y.ychar[1] = y.ychar[2];
  y.ychar[2] = c;

  return ( y.yfloat );
}
//****************************************************************************80

int float_write ( FILE *fileout, float float_val )

//****************************************************************************80
//
//  Purpose:
//
//    FLOAT_WRITE writes 1 float to a binary file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    23 September 1998
//
//  Author:
//
//    John Burkardt
//
{
  int nbyte = sizeof ( float );
  float temp;

  if ( byte_swap )
  {
    temp = float_reverse_bytes ( float_val );
  }
  else
  {
    temp = float_val;
  }

  fwrite ( &temp, nbyte, 1, fileout );

  return nbyte;
}
//****************************************************************************80

bool gmod_arch_check ( )

//****************************************************************************80
//
//  Purpose:
//
//    GMOD_ARCH_CHECK inquires into some features of the computer architecture.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    02 September 2003
//
//  Author:
//
//    Zik Saleeba
//
//  Parameters:
//
//    Output, bool GMOD_ARCH_CHECK is TRUE if the size of a float is four bytes
//    and we use IEEE floats (either big-endian or little-endian).
//
{
  static unsigned char one[4];
  int temp;

  temp = sizeof ( float );
  if ( temp != 4 )
  {
    return false;
  }

  *(float *)one = 1.0;
//
//  Little endian IEEE floats.
//
  if ( one[0] == 0 && one[1] == 0 && one[2] == 128 && one[3] == 63 )
  {
    return true;
  }
//
//  Big endian IEEE floats.
//
  if ( one[0] == 63 && one[1] == 128 && one[2] == 0 && one[3] == 0 )
  {
    return true;
  }

  return false;
}
//****************************************************************************80

int gmod_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    GMOD_READ reads a golgotha GMOD file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 May 1999
//
//  Author:
//
//    Zik Saleeba
//

//
//golgotha GMOD file format:
//
//
//        FILE HEADER
//
//w32     magic number           f9 fa 63 1e
//w32     number of sections
//[ number of sections
//    w32     section id
//    w32     section offset
//]
//
//
//        TEXTURE NAME SECTION - section id = 0x13 (19)
//
//w16     number of faces
//[ number of faces
//    w16     texture name length
//    [ texture name length
//        w8      texture name character
//    ]
//]
//
//
//
//        MODEL QUADS SECTION - section id = 0x12 (18)
//
//w16     number of faces
//[ number of faces
//    [ four vertices
//        w16     vertex index
//        float   xpos (0.0-1.0)
//        float   ypos (0.0-1.0)
//    ]
//    float   scale
//    w16     flags
//    float   xnormal     (normal should be normalised)
//    float   ynormal
//    float   znormal
//]
//
//
//        VERTEX ARRAY SECTION - section id = 0x14 (20)
//
//w16     number of vertices
//w16     number of animations
//w16     length of animation name
//[ length of animation name
//    w8      animation name character
//]
//w16     number of frames in animation
//[ number of frames in animation
//    [ number of vertices
//        float   xpos
//        float   ypos
//        float   zpos
//        float   xnormal
//        float   ynormal
//        float   znormal
//    ]
//]
//
{
  unsigned char MagicNumber[4];
  unsigned long int NumSections;
  int SectionCount;
  unsigned long int SectionID[GMOD_MAX_SECTIONS];
  unsigned long int SectionOffset[GMOD_MAX_SECTIONS];

  unsigned short NumAnimations;
  unsigned short NumFrames;
  unsigned short FaceCount;
  unsigned short TextureCount;
  int VertexCount;

  float Scale;
  unsigned short Flags;
  unsigned short TextureNameLen;
  unsigned short AnimationNameLen;
  int Order;
  int MaxCor = 0;
//
//  Check if we can handle this architecture
//
  if ( !gmod_arch_check ( ) )
  {
    cout << "\n";
    cout << "GMOD_READ - This architecture not supported.\n";
    return 1;
  }

//
//  Read the file header
//
//
//  Read the magic number
//
  fread(MagicNumber, 1, 4, filein);
  if ( MagicNumber[0] != 0xf9 ||
      MagicNumber[1] != 0xfa ||
      MagicNumber[2] != 0x63 ||
      MagicNumber[3] != 0x1e)
  {
    cout << "GMOD_READ - Bad magic number on GMOD file.\n";
    return 1;
  }

  NumSections = gmod_read_w32(filein);
  if ( GMOD_MAX_SECTIONS <= NumSections )
  {
    cout << "\n";
    cout << "GMOD_READ - Fatal error!\n";
    cout << "  Too many sections (" << NumSections << ") in GMOD file.\n";
    cout << "  Please increase static limit GMOD_MAX_SECTIONS\n";
    return 1;
  }

//
//  Read the sections.
//

  for ( SectionCount = 0; SectionCount < ( int ) NumSections; SectionCount++ )
  {
    SectionID[SectionCount] = gmod_read_w32(filein);
    SectionOffset[SectionCount] = gmod_read_w32(filein);
  }
//
//  Read each successive section.
//
  for ( SectionCount = 0; SectionCount < ( int ) NumSections; SectionCount++ )
  {
//
//  Go to the start of the section.
//
    fseek ( filein, ( long int ) SectionOffset[SectionCount], SEEK_SET );
//
//  What type of section is it?
//
    switch (SectionID[SectionCount])
    {
//
//  Model section.
//
      case G1_SECTION_MODEL_QUADS:
//
//  Get the number of faces.
//
        face_num = gmod_read_w16 ( filein );

        if ( FACE_MAX < face_num )
        {
          cout << "\n";
          cout << "GMOD_READ - Fatal error!\n";
          cout << "  Too many faces (" << face_num << ") in GMOD file.\n";
          cout << "  Increase the size of FACE_MAX.\n";
          return 1;
        }
//
//  Get the information on each face.
//
        for ( FaceCount = 0; FaceCount < ( unsigned short ) face_num; FaceCount++ )
        {
          Order = 0;
          for ( VertexCount = 0; VertexCount < 4; VertexCount++ )
          {
//
//  Read the vertex index
//
            face[VertexCount][FaceCount] = gmod_read_w16(filein);

            if (face[VertexCount][FaceCount] != GMOD_UNUSED_VERTEX)
            {
              Order = VertexCount+1;
              if ( MaxCor < face[VertexCount][FaceCount] )
              {
                MaxCor = face[VertexCount][FaceCount];
              }
            }
//
//  Read the texture position.
//
            vertex_tex_uv[0][VertexCount][FaceCount] = gmod_read_float(filein);
            vertex_tex_uv[1][VertexCount][FaceCount] = gmod_read_float(filein);
          }
//
//  Scale and flags.
//
          fread(&Scale, sizeof(Scale), 1, filein);
          Flags = gmod_read_w16(filein);

          if ( debug )
          {
            cout << "Flags = " << Flags << "\n";
          }
//
//  Normal vector.
//
          face_normal[0][FaceCount] = gmod_read_float(filein);
          face_normal[1][FaceCount] = gmod_read_float(filein);
          face_normal[2][FaceCount] = gmod_read_float(filein);
//
//  The order is the number of used vertices.
//
          face_order[FaceCount] = Order;
        }
        break;
//
//  Texture name section.
//
      case G1_SECTION_MODEL_TEXTURE_NAMES:
//
//  Get the number of textures.
//
        texture_num = gmod_read_w16(filein);
        if ( TEXTURE_MAX < texture_num )
        {
          cout << "GMOD_READ - Fatal error!\n";
          cout << "  Too many texture maps (" << texture_num << ").\n";
          cout << "  Increase static limit TEXTURE_MAX.\n";
          return 1;
        }
        face_num = texture_num;

        for (TextureCount = 0; TextureCount < ( unsigned short ) texture_num;
          TextureCount++)
        {
//
//  read the texture name
//
          TextureNameLen = gmod_read_w16(filein);
          fread ( texture_name[TextureCount], sizeof(char), TextureNameLen, filein);
          texture_name[TextureCount][TextureNameLen] = '\0';
        }
        break;
//
//  Vertex section.
//
      case G1_SECTION_MODEL_VERT_ANIMATION:
//
//  Get the number of vertices.
//
        cor3_num = gmod_read_w16(filein);
        if ( COR3_MAX < cor3_num )
        {
          cout << "GMOD_READ - Fatal error!\n";
          cout << "  Too many vertices (" << cor3_num << ").\n";
          cout << "  Increase static limit COR3_MAX.\n";
          return 1;
        }
//
//  Get the number of animations.
//
        NumAnimations = gmod_read_w16(filein);

        if ( 1 < NumAnimations )
        {
          cout << "\n";
          cout << "GMOD_READ - Fatal error!\n";
          cout << "  GMOD files can only handle one animation.\n";
          cout << "  This file contains " << NumAnimations << ".\n";
          return 1;
        }
//
//  Read the animation name.
//
        AnimationNameLen = gmod_read_w16(filein);
        fread ( anim_name, sizeof(char), AnimationNameLen, filein);
        anim_name[AnimationNameLen] = '\0';
//
//  Get the number of frames of animation.
//
        NumFrames = gmod_read_w16(filein);
        if ( 1 < NumFrames )
        {
          cout << "\n";
          cout << "GMOD_READ - Warning!\n";
          cout << "  Too many frames of animation (" << NumFrames << ").\n";
          cout << "  Will only use 1.\n";
        }
//
//  Go through all the vertices, reading each one
//
        for (VertexCount = 0; VertexCount < cor3_num; VertexCount++)
        {
//
//  Read the vertex.
//
          cor3[0][VertexCount] = gmod_read_float(filein);
          cor3[1][VertexCount] = gmod_read_float(filein);
          cor3[2][VertexCount] = gmod_read_float(filein);
//
//  Read the normal.
//
          cor3_normal[0][VertexCount] = gmod_read_float(filein);
          cor3_normal[1][VertexCount] = gmod_read_float(filein);
          cor3_normal[2][VertexCount] = gmod_read_float(filein);
        }
        break;

      default:
        continue;
    }
  }

//
//  Set some other stray info.
//
  line_num = 0;

//
//  Check for sanity.
//
  if ( cor3_num <= MaxCor )
  {
    cout << "\n";
    cout << "GMOD_READ - Fatal error!\n";
    cout << "  Maximum coordinate index (" << MaxCor << ")\n";
    cout << "  exceeds number of coordinates (" << cor3_num << ").\n";
    return 1;
  }

  return 0;
}
//****************************************************************************80

float gmod_read_float ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    GMOD_READ_FLOAT reads a float from a Golgotha GMOD file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 May 1999
//
//  Author:
//
//    Zik Saleeba
//
{
  int endian = 1;
  unsigned char *out_pos;
  int i;
  float Val;
//
//  We're little-endian, which is native for GMOD floats.
//
  if (*(char *)&endian == 1)
  {
    fread(&Val, sizeof(Val), 1, filein);
  }
//
//  We're big-endian.  Flip them.
//
  else
 {
    out_pos = (unsigned char *)&Val;
    for ( i = sizeof(Val)-1; 0 <= i ; i-- )
    {
      *(out_pos+i) = fgetc(filein);
    }
  }

  return Val;
}
//****************************************************************************80

unsigned short gmod_read_w16 ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    GMOD_READ_W16 reads a 16 bit word from a Golgotha GMOD file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 May 1999
//
//  Author:
//
//    Zik Saleeba
//
{
  unsigned char Byte1;
  unsigned char Byte2;

  Byte1 = fgetc ( filein );
  Byte2 = fgetc ( filein );

  return Byte1 | (((unsigned short)Byte2) << 8);
}
//****************************************************************************80

unsigned long gmod_read_w32 ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    GMOD_READ_W32 reads a 32 bit word from a Golgotha GMOD file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 May 1999
//
//  Author:
//
//    Zik Saleeba
//
{
  unsigned char Byte1, Byte2, Byte3, Byte4;

  Byte1 = fgetc(filein);
  Byte2 = fgetc(filein);
  Byte3 = fgetc(filein);
  Byte4 = fgetc(filein);

  return Byte1 |
         (((unsigned long)Byte2) << 8) |
         (((unsigned long)Byte3) << 16) |
         (((unsigned long)Byte4) << 24);
}
//****************************************************************************80

int gmod_write ( FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    GMOD_WRITE writes a Golgotha GMOD file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 May 1999
//
//  Author:
//
//    Zik Saleeba
//
{
  static unsigned char MagicNumber[4] = { 0xf9, 0xfa, 0x63, 0x1e };
  unsigned long NumSections;
  unsigned long SectionHeaderPos;
  unsigned long TextureNameSectionPos;
  unsigned long ModelSectionPos;
  unsigned long VertexSectionPos;

  int VertexCount;
  int FaceCount;
  int TextureCount;
  unsigned long SectionCount;
  float Scale;
  float Min[3];
  float Max[3];
  int CorNumber;
  int DimensionCount;
  float MaxWidth;
//
//  Check if we can handle this architecture.
//
  if ( !gmod_arch_check ( ) )
  {
    cout << "\n";
    cout << "GMOD_WRITE - Fatal error!\n";
    cout << "  This architecture not supported.\n";
    return 1;
  }
//
//  Write the file header.
//

//
//  Write the magic number.
//
  fwrite ( MagicNumber, sizeof(char), 4, fileout );
//
//  Write the number of sections.
//
  NumSections = 3;
  gmod_write_w32 ( NumSections, fileout );
//
//  Write a dummy section header which we'll overwrite later.
//
  SectionHeaderPos = ftell ( fileout );
  for (SectionCount = 0; SectionCount < NumSections; SectionCount++)
  {
    gmod_write_w32 ( 0, fileout );
    gmod_write_w32 ( 0, fileout );
  }
//
//  Texture name section.
//

//
//  Take note of where we are in the file.
//
  TextureNameSectionPos = ftell ( fileout );
//
//  Write the number of textures.
//
  gmod_write_w16 ( ( unsigned short ) face_num, fileout );
//
//  Write the texture names.
//
  for ( TextureCount = 0; TextureCount < face_num; TextureCount++ )
  {
    gmod_write_w16 ( ( unsigned short ) strlen ( texture_name[TextureCount] ),
      fileout );

    fwrite ( texture_name[TextureCount], strlen ( texture_name[TextureCount] ),
      1, fileout );
  }
//
//  Model section.
//

//
//  Take note of where we are in the file.
//

  ModelSectionPos = ftell(fileout);

//
//  Write the number of faces.
//
  gmod_write_w16 ( ( unsigned short ) face_num, fileout );
//
//  Write the information on each face.
//
  for ( FaceCount = 0; FaceCount < face_num; FaceCount++ )
  {

    for (VertexCount = 0; VertexCount < ((face_order[FaceCount] < 4) ? face_order[FaceCount] : 4); VertexCount++)
    {
//
//  Write the vertex index.
//
      gmod_write_w16 ( ( unsigned short ) face[VertexCount][FaceCount], fileout );
//
//  Write the texture position.
//
      gmod_write_float ( vertex_tex_uv[0][VertexCount][FaceCount], fileout );
      gmod_write_float ( vertex_tex_uv[1][VertexCount][FaceCount], fileout );
    }

//
//  Write any extra vertices which are unused.
//
    for ( ; VertexCount < 4; VertexCount++ )
    {
//
//  Write the vertex index.
//
      gmod_write_w16 ( GMOD_UNUSED_VERTEX, fileout );
//
//  Write the texture position.
//
      gmod_write_float ( vertex_tex_uv[0][VertexCount][FaceCount], fileout );

      gmod_write_float ( vertex_tex_uv[1][VertexCount][FaceCount], fileout );
    }
//
//  Scale and flags.
//

//
//  Find the bounding box.
//
    for ( DimensionCount = 0; DimensionCount < 3; DimensionCount++ )
    {
      CorNumber = face[0][FaceCount];
      Min[DimensionCount] = cor3[DimensionCount][CorNumber];
      Max[DimensionCount] = cor3[DimensionCount][CorNumber];

      for (VertexCount = 1; VertexCount < ((face_order[FaceCount] < 4) ? face_order[FaceCount] : 4); VertexCount++)
      {

        CorNumber = face[VertexCount][FaceCount];

        if ( cor3[DimensionCount][CorNumber] < Min[DimensionCount] )
          Min[DimensionCount] = cor3[DimensionCount][CorNumber];

        if (Max[DimensionCount] < cor3[DimensionCount][CorNumber])
          Max[DimensionCount] = cor3[DimensionCount][CorNumber];
      }
    }
//
//  The scale is the "width" of the face for mipmapping -
//  I just take the maximum bounding box dimension.
//
    MaxWidth = Max[0] - Min[0];
    for ( DimensionCount = 1; DimensionCount < 3; DimensionCount++ )
    {

       if ( MaxWidth < Max[DimensionCount] - Min[DimensionCount] )
           MaxWidth = Max[DimensionCount] - Min[DimensionCount];
    }
    Scale = MaxWidth;
    fwrite ( &Scale, sizeof(Scale), 1, fileout );
//
//  Flags are just nothing.
//
    gmod_write_w16 ( 0, fileout );
//
//  Normal vector.
//
    gmod_write_float ( face_normal[0][FaceCount], fileout );
    gmod_write_float ( face_normal[1][FaceCount], fileout );
    gmod_write_float ( face_normal[2][FaceCount], fileout );
  }
//
//  Vertex section.
//

//
//  Take note of where we are in the file.
//
  VertexSectionPos = ftell ( fileout );
//
//  Write the number of vertices.
//
  gmod_write_w16 ( ( unsigned short ) cor3_num, fileout );
//
//  Write the number of animations.
//
  gmod_write_w16 ( 1, fileout );
//
//  Write the animation name.
//
  gmod_write_w16 ( 0, fileout );
//
//  Write the number of frames of animation.
//
  gmod_write_w16 ( 1, fileout );
//
//  Go through all the vertices, writing each one.
//
  for ( VertexCount = 0; VertexCount < cor3_num; VertexCount++ )
  {
//
//  Write the vertex.
//
    gmod_write_float ( cor3[0][VertexCount], fileout );
    gmod_write_float ( cor3[1][VertexCount], fileout );
    gmod_write_float ( cor3[2][VertexCount], fileout );
//
//  Write the normal.
//
    gmod_write_float ( cor3_normal[0][VertexCount], fileout );
    gmod_write_float ( cor3_normal[1][VertexCount], fileout );
    gmod_write_float ( cor3_normal[2][VertexCount], fileout );
  }
//
//  Now rewrite the section header.
//

//
//  Go back to the section header.
//
  fseek ( fileout, ( long int ) SectionHeaderPos, SEEK_SET );
//
//  Write the texture name section header.
//
  gmod_write_w32 ( G1_SECTION_MODEL_TEXTURE_NAMES, fileout );
  gmod_write_w32 ( TextureNameSectionPos, fileout );
//
//  Write the model section header.
//
  gmod_write_w32 ( G1_SECTION_MODEL_QUADS, fileout );
  gmod_write_w32 ( ModelSectionPos, fileout );
//
//  Write the vertex section header.
//
  gmod_write_w32 ( G1_SECTION_MODEL_VERT_ANIMATION, fileout );
  gmod_write_w32 ( VertexSectionPos, fileout );

  return 0;
}
//****************************************************************************80

void gmod_write_float ( float Val, FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    GMOD_WRITE_FLOAT writes a float to a Golgotha GMOD file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 May 1999
//
//  Author:
//
//    Zik Saleeba
//
{
  int endian = 1;
  unsigned char *out_pos;
  int i;
//
//  We're little-endian, which is native for GMOD floats.
//
  if (*(char *)&endian == 1)
  {

    fwrite ( &Val, sizeof(Val), 1, fileout );
  }
//
//  We're big-endian, flip `em.
//
  else
  {
    out_pos = (unsigned char *)&Val;
    for ( i = sizeof(Val)-1; 0 <= i; i-- )
    {
      fputc(*(out_pos+i), fileout);
    }
  }
  return;
}
//****************************************************************************80

void gmod_write_w16 ( unsigned short Val, FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    GMOD_WRITE_W16 writes a 16 bit word to a Golgotha GMOD file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    13 September 2000
//
//  Author:
//
//    Zik Saleeba
//
{
  unsigned char OutByte[2];

  OutByte[0] = (unsigned char)(Val & 0xff);
  OutByte[1] = (unsigned char)(Val >> 8);

  fwrite ( OutByte, sizeof(unsigned char), 2, fileout );

  return;
}
//****************************************************************************80

void gmod_write_w32 ( unsigned long Val, FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    GMOD_WRITE writes a 32 bit word to a Golgotha GMOD file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 May 1999
//
//  Author:
//
//    Zik Saleeba
//
{
  unsigned char OutByte[4];

  OutByte[0] = (unsigned char)(Val & 0xff);
  OutByte[1] = (unsigned char)((Val >> 8) & 0xff);
  OutByte[2] = (unsigned char)((Val >> 16) & 0xff);
  OutByte[3] = (unsigned char)((Val >> 24) & 0xff);

  fwrite ( OutByte, sizeof(unsigned char), 4, fileout );

  return;
}
//****************************************************************************80

void hello ( )

//****************************************************************************80
//
//  Purpose:
//
//    HELLO prints an explanatory header message.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    04 July 2000
//
//  Author:
//
//    John Burkardt
//
{
  cout << "\n";
  cout << "Hello:  This is IVCON,\n";
  cout << "  for 3D graphics file conversion.\n";
  cout << "\n";
  cout << "    \".3ds\"   3D Studio Max binary;\n";
  cout << "    \".ase\"   3D Studio Max ASCII export;\n";
  cout << "    \".byu\"   Movie.BYU surface geometry;\n";
  cout << "    \".dxf\"   DXF;\n";
  cout <<  "    \".gmod\"  Golgotha model;\n";
  cout << "    \".hrc\"   SoftImage hierarchy;\n";
  cout << "    \".iv\"    SGI Open Inventor;\n";
  cout << "    \".obj\"   WaveFront Advanced Visualizer;\n";
  cout << "    \".off\"   GEOMVIEW Object File Format;\n";
  cout << "    \".pov\"   Persistence of Vision (output only);\n";
  cout << "    \".smf\"   Michael Garland's format;\n";
  cout << "    \".stl\"   ASCII StereoLithography;\n";
  cout << "    \".stla\"  ASCII StereoLithography;\n";
  cout << "    \".stlb\"  Binary StereoLithography;\n";
  cout << "    \".tec\"   TECPLOT (output only);\n";
  cout << "    \".tri\"   [Greg Hood ASCII triangle format];\n";
  cout << "    \".tria\"  [Greg Hood ASCII triangle format];\n";
  cout << "    \".trib\"  [Greg Hood binary triangle format];\n";
  cout << "    \".txt\"   Text (output only);\n";
  cout << "    \".ucd\"   AVS UCD file(output only);\n";
  cout << "    \".vla\"   VLA;\n";
  cout << "    \".wrl\"   VRML (Virtual Reality Modeling Language) (output only).\n";
  cout << "    \".xgl\"   XML/OpenGL format (output only);\n";
  cout << "\n";
  cout << "  Current limits include:\n";
  cout << "    " << FACE_MAX << " faces.\n";
  cout << "    " << LINES_MAX << " line items.\n";
  cout << "    " << COR3_MAX << " points.\n";
  cout << "    " << ORDER_MAX << " face order.\n";
  cout << "    " << MATERIAL_MAX << " materials.\n";
  cout << "    " << TEXTURE_MAX << " textures.\n";
  cout << "\n";
  cout << "  Last modification: 04 September 2003.\n";

  return;
}
//****************************************************************************80

void help ( )

//****************************************************************************80
//
//  Purpose:
//
//    HELP prints a list of the interactive commands.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    26 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  cout << "\n";
  cout << "Commands:\n";
  cout << "\n";
  cout << "< file   Read data from input file;\n";
  cout << "<< file  Append data in input file to current data;\n";
  cout << "> file   Write output file;\n";
  cout << "B        Switch the binary file byte-swapping mode;\n";
  cout << "D        Switch the debugging mode;\n";
  cout << "F        Print information about one face;\n";
  cout << "H        Print this help list;\n";
  cout << "I        Info, print out recent changes;\n";
  cout << "LINES    Convert face information to lines;\n";
  cout << "N        Recompute normal vectors;\n";
  cout << "P        Set LINE_PRUNE option.\n";
  cout << "Q        Quit;\n";
  cout << "R        Reverse the normal vectors.\n";
  cout << "S        Select face subset (NOT WORKING).\n";
  cout << "T        Transform the data.\n";
  cout << "W        Reverse the face node ordering.\n";

  return;
}
//****************************************************************************80

int hrc_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    HRC_READ reads graphics information from a SoftImage HRC file.
//
//  Example:
//
//    HRCH: Softimage 4D Creative Environment v3.00
//
//
//    model
//    {
//      name         "cube_10x10"
//      scaling      1.000 1.000 1.000
//      rotation     0.000 0.000 0.000
//      translation  0.000 0.000 0.000
//
//      mesh
//      {
//        flag    ( PROCESS )
//        discontinuity  60.000
//
//        vertices   8
//        {
//          [0] position  -5.000  -5.000  -5.000
//          [1] position  -5.000  -5.000  5.000
//          [2] position  -5.000  5.000  -5.000
//          [3] position  -5.000  5.000  5.000
//          [4] position  5.000  -5.000  -5.000
//          [5] position  5.000  -5.000  5.000
//          [6] position  5.000  5.000  -5.000
//          [7] position  5.000  5.000  5.000
//        }
//
//        polygons   6
//        {
//          [0] nodes  4
//              {
//                [0] vertex  0
//                    normal  -1.000  0.000  0.000
//                    uvTexture  0.000  0.000
//                    vertexColor 255 178 178 178
//                [1] vertex  1
//                    normal  -1.000  0.000  0.000
//                    uvTexture  0.000  0.000
//                    vertexColor 255 178 178 178
//                [2] vertex  3
//                    normal  -1.000  0.000  0.000
//                    uvTexture  0.000  0.000
//                    vertexColor 255 178 178 178
//                [3] vertex  2
//                    normal  -1.000  0.000  0.000
//                    uvTexture  0.000  0.000
//                    vertexColor 255 178 178 178
//              }
//              material  0
//          [1] nodes  4
//             {
//                [0] vertex  1
//                    normal  0.000  0.000  1.000
//                    uvTexture  0.000  0.000
//                    vertexColor 255 178 178 178
//                [1] vertex  5
//
//    ...etc.....
//
//          [5] nodes  4
//              {
//                [0] vertex  2
//                    normal  0.000  1.000  0.000
//                    uvTexture  0.000  0.000
//                    vertexColor 255 178 178 178
//                [1] vertex  3
//                    normal  0.000  1.000  0.000
//                    uvTexture  0.000  0.000
//                    vertexColor 255 178 178 178
//                [2] vertex  7
//                    normal  0.000  1.000  0.000
//                    uvTexture  0.000  0.000
//                    vertexColor 255 178 178 178
//                [3] vertex  6
//                    normal  0.000  1.000  0.000
//                    uvTexture  0.000  0.000
//                    vertexColor 255 178 178 178
//              }
//              material  0
//        }
//
//        edges   12
//        {
//          [1] vertices  3  2
//          [2] vertices  2  0
//          [3] vertices  0  1
//          [4] vertices  1  3
//          [5] vertices  7  3
//          [6] vertices  1  5
//          [7] vertices  5  7
//          [8] vertices  6  7
//          [9] vertices  5  4
//          [10] vertices  4  6
//          [11] vertices  2  6
//          [12] vertices  4  0
//        }
//      }
//
//      material [0]
//      {
//      name           "kazoo"
//      type           PHONG
//      ambient        0.0  1.0  0.0
//      diffuse        1.0  0.0  0.0
//      specular       0.0  0.0  1.0
//      exponent      50.0
//      reflectivity   0.0
//      transparency   0.0
//      refracIndex    1.0
//      glow           0
//      coc            0.0
//      }
//
//      texture [0]
//      {
//      name          "/usr/users/foss/HOUSE/PICTURES/mellon"
//      glbname       "t2d1"
//      anim          STATIC
//      method        XY
//      repeat        1  1
//      scaling       1.000  1.000
//      offset        0.000  0.000
//      pixelInterp
//      effect        INTENSITY
//      blending      1.000
//      ambient       0.977
//      diffuse       1.000
//      specular      0.966
//      reflect       0.000
//      transp        0.000
//      roughness     0.000
//      reflMap       1.000
//      rotation      0.000
//      txtsup_rot    0.000  0.000  0.000
//      txtsup_trans  0.000  0.000  0.000
//      txtsup_scal   1.000  1.000  1.000
//      }
//    }
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    25 June 1999
//
//  Author:
//
//    John Burkardt
//
{
  float b;
  int   count;
  float g;
  int   i;
  int   icor3;
  int   ivert;
  int   iword;
  int   jval;
  int   level;
  char *next;
  int   nlbrack;
  int   nrbrack;
  int   cor3_num_old;
  float r;
  float t;
  float temp[3];
  int   width;
  char  word[LINE_MAX_LEN];
  char  word1[LINE_MAX_LEN];
  char  word2[LINE_MAX_LEN];
  char  wordm1[LINE_MAX_LEN];
  float x;
  float y;
  float z;

  level = 0;
  strcpy ( level_name[0], "Top" );
  nlbrack = 0;
  nrbrack = 0;
  cor3_num_old = cor3_num;
  strcpy ( word, " " );
  strcpy ( wordm1, " " );
//
//  Read a line of text from the file.
//
  for ( ;; )
  {

    if ( fgets ( input, LINE_MAX_LEN, filein ) == NULL )
    {
      break;
    }

    text_num = text_num + 1;
    next = input;
    iword = 0;
//
//  Read a word from the line.
//
    for ( ;; )
    {
      strcpy ( wordm1, word );

      count = sscanf ( next, "%s%n", word2, &width );
      next = next + width;

      if ( count <= 0 )
      {
        break;
      }

      strcpy ( word, word2 );

      iword = iword + 1;

      if ( iword == 1 )
      {
        strcpy ( word1, word );
      }
//
//  The first line of the file must be the header.
//
      if ( text_num == 1 )
      {
        if ( strcmp ( word1, "HRCH:" ) != 0 )
        {
          cout << "\n";
          cout << "HRC_READ - Fatal error!\n";
          cout << "  The input file has a bad header.\n";;
          return 1;
        }
        else
        {
          comment_num = comment_num + 1;
        }
        break;
      }
//
//  If the word is a curly bracket, count it.
//
      if ( strcmp ( word, "{" ) == 0 )
      {
        nlbrack = nlbrack + 1;
        level = nlbrack - nrbrack;
        strcpy ( level_name[level], wordm1 );
        if ( debug )
        {
          cout << "New level: " << level_name[level] << "\n";;
        }
      }
      else if ( strcmp ( word, "}" ) == 0 )
      {
        nrbrack = nrbrack + 1;

        if ( nlbrack < nrbrack )
        {
          cout << "\n";
          cout << "HRC_READ - Fatal error!\n";
          cout << "  Extraneous right bracket on line " << text_num << ".\n";
          cout << "  Processing field " << level_name[level] << "\n";
          return 1;
        }
      }
//
//  CONTROLPOINTS
//
      if ( strcmp ( level_name[level], "controlpoints" ) == 0 )
      {

        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          if ( line_num < LINES_MAX )
          {
            line_dex[line_num] = -1;
            line_material[line_num] = 0;
          }
          line_num = line_num + 1;
          level = nlbrack - nrbrack;
        }
        else if ( word[0] == '[' )
        {
        }
        else if ( strcmp ( word, "position" ) == 0 )
        {
          count = sscanf ( next, "%f%n", &x, &width );
          next = next + width;

          count = sscanf ( next, "%f%n", &y, &width );
          next = next + width;

          count = sscanf ( next, "%f%n", &z, &width );
          next = next + width;

          temp[0] = x;
          temp[1] = y;
          temp[2] = z;

          if ( cor3_num < 1000 )
          {
            icor3 = rcol_find ( cor3, 3, cor3_num, temp );
          }
          else
          {
            icor3 = -1;
          }

          if ( icor3 == -1 )
          {
            icor3 = cor3_num;
            if ( cor3_num < COR3_MAX )
            {
              cor3[0][cor3_num] = x;
              cor3[1][cor3_num] = y;
              cor3[2][cor3_num] = z;
            }
            cor3_num = cor3_num + 1;

          }
          else
          {
            dup_num = dup_num + 1;
          }

          if ( line_num < LINES_MAX )
          {
            line_dex[line_num] = icor3;
            line_material[line_num] = 0;
          }
          line_num = line_num + 1;
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "CONTROLPOINTS: Bad data " << word << "\n";
          return 1;
        }

      }
//
//  EDGES
//
      else if ( strcmp ( level_name[level], "edges" ) == 0 )
      {
        if ( strcmp( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( word[0] == '[' )
        {
        }
        else if ( strcmp ( word, "vertices" ) == 0 )
        {

          count = sscanf ( next, "%d%n", &jval, &width );
          next = next + width;

          if ( line_num < LINES_MAX )
          {
            line_dex[line_num] = jval + cor3_num_old;
            line_material[line_num] = 0;
          }
          line_num = line_num + 1;

          count = sscanf ( next, "%d%n", &jval, &width );
          next = next + width;

          if ( line_num < LINES_MAX )
          {
            line_dex[line_num] = jval + cor3_num_old;
            line_material[line_num] = 0;
          }
          line_num = line_num + 1;

          if ( line_num < LINES_MAX )
          {
            line_dex[line_num] = -1;
            line_material[line_num] = -1;
          }
          line_num = line_num + 1;

        }
        else
        {
          bad_num = bad_num + 1;
          cout << "EDGES: Bad data " << word << "\n";
          return 1;
        }

      }
//
//  MATERIAL
//
      else if ( strcmp ( level_name[level], "material" ) == 0 )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
          material_num = material_num + 1;
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( word[0] == '[' )
        {
        }
        else if ( strcmp ( word, "ambient" ) == 0 )
        {
        }
        else if ( strcmp ( word, "coc" ) == 0 )
        {
        }
        else if ( strcmp ( word, "diffuse" ) == 0 )
        {

          count = sscanf ( next, "%f%n", &r, &width );
          next = next + width;
          material_rgba[0][material_num-1] = r;

          count = sscanf ( next, "%f%n", &g, &width );
          next = next + width;
          material_rgba[0][material_num-1] = g;

          count = sscanf ( next, "%f%n", &b, &width );
          next = next + width;
          material_rgba[0][material_num-1] = b;

        }
        else if ( strcmp ( word, "exponent" ) == 0 )
        {
        }
        else if ( strcmp ( word, "glow" ) == 0 )
        {
        }
        else if ( strcmp ( word, "name" ) == 0 )
        {
          count = sscanf ( next, "%s%n", word, &width );
          next = next + width;
          strcpy ( material_name[material_num-1], word );
        }
        else if ( strcmp ( word, "reflectivity" ) == 0 )
        {
        }
        else if ( strcmp ( word, "refracindex" ) == 0 )
        {
        }
        else if ( strcmp ( word, "specular" ) == 0 )
        {
        }
        else if ( strcmp ( word, "transparency" ) == 0 )
        {
          count = sscanf ( next, "%f%n", &t, &width );
          next = next + width;
          material_rgba[3][material_num-1] = 1.0 - t;
        }
        else if ( strcmp ( word, "type" ) == 0 )
        {
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "MATERIAL: Bad data " << word << "\n";
          return 1;
        }
      }
//
//  MESH
//
      else if ( strcmp ( level_name[level], "mesh" ) == 0 )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( strcmp ( word, "discontinuity" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "edges" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "flag" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "polygons" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "vertices" ) == 0 )
        {
          break;
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "MESH: Bad data " << word << "\n";
          return 1;
        }

      }
//
//  MODEL
//
      else if ( strcmp ( level_name[level], "model" ) == 0 )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( strcmp ( word, "material" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "mesh" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "name" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "patch" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "rotation" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "scaling" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "spline" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "translation" ) == 0 )
        {
          break;
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "MODEL: Bad data " << word << "\n";
          return 1;
        }

      }
//
//  NODES
//
      else if ( strcmp ( level_name[level], "nodes" ) == 0 )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
          ivert = 0;
          face_order[face_num] = 0;
          face_num = face_num + 1;
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( word[0] == '[' )
        {
        }
        else if ( strcmp ( word, "normal" ) == 0 )
        {
          count = sscanf ( next, "%f%n", &x, &width );
          next = next + width;

          count = sscanf ( next, "%f%n", &y, &width );
          next = next + width;

          count = sscanf ( next, "%f%n", &z, &width );
          next = next + width;

          if ( ivert < ORDER_MAX && face_num < FACE_MAX )
          {
            vertex_normal[0][ivert-1][face_num-1] = x;
            vertex_normal[1][ivert-1][face_num-1] = y;
            vertex_normal[2][ivert-1][face_num-1] = z;
          }

        }
        else if ( strcmp ( word, "uvTexture" ) == 0 )
        {
          count = sscanf ( next, "%f%n", &x, &width );
          next = next + width;

          count = sscanf ( next, "%f%n", &y, &width );
          next = next + width;

          if ( ivert < ORDER_MAX && face_num < FACE_MAX )
          {
            vertex_tex_uv[0][ivert-1][face_num-1] = x;
            vertex_tex_uv[1][ivert-1][face_num-1] = y;
          }
        }
        else if ( strcmp ( word, "vertex" ) == 0 )
        {
          count = sscanf ( next, "%d%n", &jval, &width );
          next = next + width;

          if ( ivert < ORDER_MAX && face_num < FACE_MAX )
          {
            face_order[face_num-1] = face_order[face_num-1] + 1;
            face[ivert][face_num-1] = jval;
          }
          ivert = ivert + 1;

        }
//
//  Right now, we don't do anything with the vertexColor information.
//
        else if ( strcmp ( word, "vertexColor" ) == 0 )
        {
          count = sscanf ( next, "%d%n", &jval, &width );
          next = next + width;

          count = sscanf ( next, "%d%n", &jval, &width );
          next = next + width;

          count = sscanf ( next, "%d%n", &jval, &width );
          next = next + width;

          count = sscanf ( next, "%d%n", &jval, &width );
          next = next + width;
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "NODES: Bad data " << word << "\n";
          return 1;
        }

      }
//
//  PATCH
//  I don't know what to do with this yet.
//
      else if ( strcmp ( level_name[level], "patch" ) == 0 )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( strcmp ( word, "approx_type" ) == 0 )
        {
        }
        else if ( strcmp ( word, "controlpoints" ) == 0 )
        {
        }
        else if ( strcmp ( word, "curv_u" ) == 0 )
        {
        }
        else if ( strcmp ( word, "curv_v" ) == 0 )
        {
        }
        else if ( strcmp ( word, "recmin" ) == 0 )
        {
        }
        else if ( strcmp ( word, "recmax" ) == 0 )
        {
        }
        else if ( strcmp ( word, "recursion" ) == 0 )
        {
        }
        else if ( strcmp ( word, "spacial" ) == 0 )
        {
        }
        else if ( strcmp ( word, "taggedpoints" ) == 0 )
        {
        }
        else if ( strcmp ( word, "ucurve" ) == 0 )
        {
        }
        else if ( strcmp ( word, "ustep" ) == 0 )
        {
        }
        else if ( strcmp ( word, "utension" ) == 0 )
        {
        }
        else if ( strcmp ( word, "utype" ) == 0 )
        {
        }
        else if ( strcmp ( word, "vclose" ) == 0 )
        {
        }
        else if ( strcmp ( word, "vcurve" ) == 0 )
        {
        }
        else if ( strcmp ( word, "viewdep" ) == 0 )
        {
        }
        else if ( strcmp ( word, "vpoint" ) == 0 )
        {
        }
        else if ( strcmp ( word, "vstep" ) == 0 )
        {
        }
        else if ( strcmp ( word, "vtension" ) == 0 )
        {
        }
        else if ( strcmp ( word, "vtype" ) == 0 )
        {
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "PATCH: Bad data " << word << "\n";
          return 1;
        }
      }
//
//  POLYGONS
//
      else if ( strcmp ( level_name[level], "polygons" ) == 0 )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( word[0] == '[' )
        {
        }
        else if ( strcmp ( word, "material" ) == 0 )
        {
          count = sscanf ( next, "%d%n", &jval, &width );
          next = next + width;

          for ( ivert = 0; ivert < ORDER_MAX; ivert++ )
          {
            vertex_material[ivert][face_num-1] = jval;
          }

        }
        else if ( strcmp ( word, "nodes" ) == 0 )
        {
          count = sscanf ( next, "%s%n", word2, &width );
          next = next + width;
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "POLYGONS: Bad data " << word << "\n";
          return 1;
        }

      }
//
//  SPLINE
//
      else if ( strcmp ( level_name[level], "spline" ) == 0 )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( strcmp ( word, "controlpoints" ) == 0 )
        {
          break;
        }
//
//  WHY DON'T YOU READ IN THE OBJECT NAME HERE?
//
        else if ( strcmp ( word, "name" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "nbKeys" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "step" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "tension" ) == 0 )
        {
          break;
        }
        else if ( strcmp ( word, "type" ) == 0 )
        {
          break;
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "SPLINE: Bad data " << word << "\n";
          return 1;
        }

      }
//
//  TAGGEDPOINTS
//
      else if ( strcmp ( level_name[level], "taggedpoints" ) == 0 )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( word[0] == '[' )
        {
        }
        else if ( strcmp ( word, "tagged" ) == 0 )
        {
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "TAGGEDPOINTS: Bad data " << word << "\n";
          return 1;
        }

      }
//
//  TEXTURE
//
      else if ( strcmp ( level_name[level], "texture" ) == 0 )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
          texture_num = texture_num + 1;
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( word[0] == '[' )
        {
        }
        else if ( strcmp ( word, "ambient" ) == 0 )
        {
        }
        else if ( strcmp ( word, "anim" ) == 0 )
        {
        }
        else if ( strcmp ( word, "blending" ) == 0 )
        {
        }
        else if ( strcmp ( word, "diffuse" ) == 0 )
        {
        }
        else if ( strcmp ( word, "effect" ) == 0 )
        {
        }
        else if ( strcmp ( word, "glbname" ) == 0 )
        {
        }
        else if ( strcmp ( word, "method" ) == 0 )
        {
        }
        else if ( strcmp ( word, "name" ) == 0 )
        {
          count = sscanf ( next, "%s%n", word, &width );
          next = next + width;
          strcpy ( texture_name[texture_num-1], word );
        }
        else if ( strcmp ( word, "offset" ) == 0 )
        {
        }
        else if ( strcmp ( word, "pixelinterp" ) == 0 )
        {
        }
        else if ( strcmp ( word, "reflect" ) == 0 )
        {
        }
        else if ( strcmp ( word, "reflmap" ) == 0 )
        {
        }
        else if ( strcmp ( word, "repeat" ) == 0 )
        {
        }
        else if ( strcmp ( word, "rotation" ) == 0 )
        {
        }
        else if ( strcmp ( word, "roughness" ) == 0 )
        {
        }
        else if ( strcmp ( word, "scaling" ) == 0 )
        {
        }
        else if ( strcmp ( word, "specular" ) == 0 )
        {
        }
        else if ( strcmp ( word, "transp" ) == 0 )
        {
        }
        else if ( strcmp ( word, "txtsup_rot" ) == 0 )
        {
        }
        else if ( strcmp ( word, "txtsup_scal" ) == 0 )
        {
        }
        else if ( strcmp ( word, "txtsup_trans" ) == 0 )
        {
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "TEXTURE: Bad data " << word << "\n";
          return 1;
        }
      }
//
//  VERTICES
//
      else if ( strcmp ( level_name[level], "vertices" ) == 0 )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( word[0] == '[' )
        {
        }
        else if ( strcmp ( word, "position" ) == 0 )
        {
          count = sscanf ( next, "%f%n", &x, &width );
          next = next + width;

          count = sscanf ( next, "%f%n", &y, &width );
          next = next + width;

          count = sscanf ( next, "%f%n", &z, &width );
          next = next + width;

          if ( cor3_num < COR3_MAX )
          {
            cor3[0][cor3_num] = x;
            cor3[1][cor3_num] = y;
            cor3[2][cor3_num] = z;
          }
          cor3_num = cor3_num + 1;
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "VERTICES: Bad data " << word << "\n";
          return 1;
        }
      }
//
//  Any other word:
//
      else
      {
      }
    }
  }

//
//  End of information in file.
//
//  Check the "materials" defining a line.
//
//  If COORDINDEX is -1, so should be the MATERIALINDEX.
//  If COORDINDEX is not -1, then the MATERIALINDEX shouldn"t be either.
//
  for ( i = 0; i < line_num; i++ )
  {
    if ( line_dex[i] == -1 )
    {
      line_material[i] = -1;
    }
    else if ( line_material[i] == -1 )
    {
      line_material[i] = 0;
    }

  }
  return 0;
}
//****************************************************************************80

int hrc_write ( FILE* fileout )

//****************************************************************************80
//
//  Purpose:
//
//    HRC_WRITE writes graphics data to an HRC SoftImage file.
//
//  Example:
//
//    HRCH: Softimage 4D Creative Environment v3.00
//
//
//    model
//    {
//      name         "cube_10x10"
//      scaling      1.000 1.000 1.000
//      rotation     0.000 0.000 0.000
//      translation  0.000 0.000 0.000
//
//      mesh
//      {
//        flag    ( PROCESS )
//        discontinuity  60.000
//
//        vertices   8
//        {
//          [0] position  -5.000  -5.000  -5.000
//          [1] position  -5.000  -5.000  5.000
//          [2] position  -5.000  5.000  -5.000
//          [3] position  -5.000  5.000  5.000
//          [4] position  5.000  -5.000  -5.000
//          [5] position  5.000  -5.000  5.000
//          [6] position  5.000  5.000  -5.000
//          [7] position  5.000  5.000  5.000
//        }
//
//        polygons   6
//        {
//          [0] nodes  4
//              {
//                [0] vertex  0
//                    normal  -1.000  0.000  0.000
//                    uvTexture  0.000  0.000
//                    vertexColor 255 178 178 178
//                [1] vertex  1
//                    normal  -1.000  0.000  0.000
//                    uvTexture  0.000  0.000
//                    vertexColor 255 178 178 178
//                [2] vertex  3
//                    normal  -1.000  0.000  0.000
//                    uvTexture  0.000  0.000
//                    vertexColor 255 178 178 178
//                [3] vertex  2
//                    normal  -1.000  0.000  0.000
//                    uvTexture  0.000  0.000
//                    vertexColor 255 178 178 178
//              }
//              material  0
//          [1] nodes  4
//             {
//                [0] vertex  1
//                    normal  0.000  0.000  1.000
//                    uvTexture  0.000  0.000
//                    vertexColor 255 178 178 178
//                [1] vertex  5
//
//    ...etc.....
//
//          [5] nodes  4
//              {
//                [0] vertex  2
//                    normal  0.000  1.000  0.000
//                    uvTexture  0.000  0.000
//                    vertexColor 255 178 178 178
//                [1] vertex  3
//                    normal  0.000  1.000  0.000
//                    uvTexture  0.000  0.000
//                    vertexColor 255 178 178 178
//                [2] vertex  7
//                    normal  0.000  1.000  0.000
//                    uvTexture  0.000  0.000
//                    vertexColor 255 178 178 178
//                [3] vertex  6
//                    normal  0.000  1.000  0.000
//                    uvTexture  0.000  0.000
//                    vertexColor 255 178 178 178
//              }
//              material  0
//        }
//
//        edges   12
//        {
//          [1] vertices  3  2
//          [2] vertices  2  0
//          [3] vertices  0  1
//          [4] vertices  1  3
//          [5] vertices  7  3
//          [6] vertices  1  5
//          [7] vertices  5  7
//          [8] vertices  6  7
//          [9] vertices  5  4
//          [10] vertices  4  6
//          [11] vertices  2  6
//          [12] vertices  4  0
//        }
//      }
//
//      material [0]
//      {
//      name           "kazoo"
//      type           PHONG
//      ambient        0.0  1.0  0.0
//      diffuse        1.0  0.0  0.0
//      specular       0.0  0.0  1.0
//      exponent      50.0
//      reflectivity   0.0
//      transparency   0.0
//      refracIndex    1.0
//      glow           0
//      coc            0.0
//      }
//
//      texture [0]
//      {
//      name          "/usr/users/foss/HOUSE/PICTURES/mellon"
//      glbname       "t2d1"
//      anim          STATIC
//      method        XY
//      repeat        1  1
//      scaling       1.000  1.000
//      offset        0.000  0.000
//      pixelInterp
//      effect        INTENSITY
//      blending      1.000
//      ambient       0.977
//      diffuse       1.000
//      specular      0.966
//      reflect       0.000
//      transp        0.000
//      roughness     0.000
//      reflMap       1.000
//      rotation      0.000
//      txtsup_rot    0.000  0.000  0.000
//      txtsup_trans  0.000  0.000  0.000
//      txtsup_scal   1.000  1.000  1.000
//      }
//    }
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    25 June 1998
//
//  Author:
//
//    John Burkardt
//
//
{
  int iface;
  int ivert;
  int j;
  int jhi;
  int jlo;
  int jrel;
  int k;
  int npts;
  int nseg;
  int text_num;

  nseg = 0;
  text_num = 0;

  fprintf ( fileout, "HRCH: Softimage 4D Creative Environment v3.00\n" );
  fprintf ( fileout, "\n" );
  fprintf ( fileout, "\n" );
  text_num = text_num + 3;

  fprintf ( fileout, "model\n" );
  fprintf ( fileout, "{\n" );
  fprintf ( fileout, "  name         \"%s\"\n", object_name );
  fprintf ( fileout, "  scaling      1.000 1.000 1.000\n" );
  fprintf ( fileout, "  rotation     0.000 0.000 0.000\n" );
  fprintf ( fileout, "  translation  0.000 0.000 0.000\n" );
  text_num = text_num + 6;

  if ( 0 < face_num )
  {
    fprintf ( fileout, "\n" );
    fprintf ( fileout, "  mesh\n" );
    fprintf ( fileout, "  {\n" );
    fprintf ( fileout, "    flag    ( PROCESS )\n" );
    fprintf ( fileout, "    discontinuity  60.000\n" );
    text_num = text_num + 5;
//
//  Point coordinates.
//
    if ( 0 < cor3_num )
    {
      fprintf ( fileout, "\n" );
      fprintf ( fileout, "    vertices %d\n", cor3_num );
      fprintf ( fileout, "    {\n" );
      text_num = text_num + 3;

      for ( j = 0; j < cor3_num; j++ )
      {
        fprintf ( fileout, "      [%d] position %f %f %f\n", j, cor3[0][j],
          cor3[1][j], cor3[2][j] );
        text_num = text_num + 1;
      }
      fprintf ( fileout, "    }\n" );
      text_num = text_num + 1;
    }
//
//  Faces.
//
    fprintf ( fileout, "\n" );
    fprintf ( fileout, "    polygons %d\n", face_num );
    fprintf ( fileout, "    {\n" );
    text_num = text_num + 3;

    for ( iface = 0; iface < face_num; iface++ )
    {
      fprintf ( fileout, "      [%d] nodes %d\n", iface, face_order[iface] );
      fprintf ( fileout, "      {\n" );
      text_num = text_num + 2;

      for ( ivert = 0; ivert < face_order[iface]; ivert++ )
      {
        fprintf ( fileout, "        [%d] vertex %d\n", ivert, face[ivert][iface] );
        fprintf ( fileout, "            normal %f %f %f\n",
          vertex_normal[0][ivert][iface],
          vertex_normal[1][ivert][iface], vertex_normal[2][ivert][iface] );
        fprintf ( fileout, "            uvTexture  %f %f\n",
          vertex_tex_uv[0][ivert][iface], vertex_tex_uv[1][ivert][iface] );
        fprintf ( fileout, "            vertexColor  255 178 178 178\n" );
        text_num = text_num + 4;
      }
      fprintf ( fileout, "      }\n" );
      fprintf ( fileout, "      material %d\n", face_material[iface] );
      text_num = text_num + 2;
    }
    fprintf ( fileout, "    }\n" );
    fprintf ( fileout, "  }\n" );
    text_num = text_num + 2;
  }
//
//  IndexedLineSet.
//
  if ( 0 < line_num )
  {
    nseg = 0;

    jhi = -1;

    for ( ;; )
    {
      jlo = jhi + 1;
//
//  Look for the next index JLO that is not -1.
//
      while ( jlo < line_num )
      {
        if ( line_dex[jlo] != -1 )
        {
          break;
        }
        jlo = jlo + 1;
      }

      if ( line_num <= jlo )
      {
        break;
      }
//
//  Look for the highest following index JHI that is not -1.
//
      jhi = jlo + 1;

      while ( jhi < line_num )
      {
        if ( line_dex[jhi] == -1 )
        {
          break;
        }
        jhi = jhi + 1;
      }

      jhi = jhi - 1;
//
//  Our next line segment involves LINE_DEX indices JLO through JHI.
//
      nseg = nseg + 1;
      npts = jhi + 1 - jlo;

      fprintf ( fileout, "\n" );
      fprintf ( fileout, "  spline\n" );
      fprintf ( fileout, "  {\n" );
      fprintf ( fileout, "    name     \"spl%d\"\n", nseg );
      fprintf ( fileout, "    type     LINEAR\n" );
      fprintf ( fileout, "    nbKeys   %d\n", npts );
      fprintf ( fileout, "    tension  0.000\n" );
      fprintf ( fileout, "    step     1\n" );
      fprintf ( fileout, "\n" );
      text_num = text_num + 9;

      fprintf ( fileout, "    controlpoints\n" );
      fprintf ( fileout, "    {\n" );
      text_num = text_num + 2;

      for ( j = jlo; j <= jhi; j++ )
      {
        jrel = j - jlo;
        k = line_dex[j];
        fprintf ( fileout, "      [%d] position %f %f %f\n", jrel,
          cor3[0][k], cor3[1][k], cor3[2][k] );
        text_num = text_num + 1;
      }

      fprintf ( fileout, "    }\n" );
      fprintf ( fileout, "  }\n" );
      text_num = text_num + 2;
    }
  }
//
//  MATERIALS
//
  for ( i = 0; i < material_num; i++ )
  {
    fprintf ( fileout, "  material [%d]\n", i );
    fprintf ( fileout, "  {\n" );
    fprintf ( fileout, "    name           \"%s\"\n", material_name[i] );
    fprintf ( fileout, "    type           PHONG\n" );
    fprintf ( fileout, "    ambient        %f %f %f\n", material_rgba[0][i],
      material_rgba[1][i], material_rgba[2][i] );
    fprintf ( fileout, "    diffuse        %f %f %f\n", material_rgba[0][i],
      material_rgba[1][i], material_rgba[2][i] );
    fprintf ( fileout, "    specular       %f %f %f\n", material_rgba[0][i],
      material_rgba[1][i], material_rgba[2][i] );
    fprintf ( fileout, "    exponent      50.0\n" );
    fprintf ( fileout, "    reflectivity   0.0\n" );
    fprintf ( fileout, "    transparency   %f\n", 1.0 - material_rgba[3][i] );
    fprintf ( fileout, "    refracIndex    1.0\n" );
    fprintf ( fileout, "    glow           0\n" );
    fprintf ( fileout, "    coc            0.0\n" );
    fprintf ( fileout, "  }\n" );

    text_num = text_num + 14;

  }
//
//  TEXTURES
//
  for ( i = 0; i < texture_num; i++ )
  {
    fprintf ( fileout, "  texture [%d]\n", i );
    fprintf ( fileout, "  {\n" );
    fprintf ( fileout, "    name           \"%s\"\n", texture_name[i] );
    fprintf ( fileout, "    glbname        \"t2d1\"\n" );
    fprintf ( fileout, "    anim           STATIC\n" );
    fprintf ( fileout, "    method         XY\n" );
    fprintf ( fileout, "    repeat         1 1\n" );
    fprintf ( fileout, "    scaling        1.000  1.000\n" );
    fprintf ( fileout, "    offset         0.000  0.000\n" );
    fprintf ( fileout, "    pixelInterp\n" );
    fprintf ( fileout, "    effect         INTENSITY\n" );
    fprintf ( fileout, "    blending       1.000\n" );
    fprintf ( fileout, "    ambient        0.977\n" );
    fprintf ( fileout, "    diffuse        1.000\n" );
    fprintf ( fileout, "    specular       0.966\n" );
    fprintf ( fileout, "    reflect        0.000\n" );
    fprintf ( fileout, "    transp         0.000\n" );
    fprintf ( fileout, "    roughness      0.000\n" );
    fprintf ( fileout, "    reflMap        1.000\n" );
    fprintf ( fileout, "    rotation       0.000\n" );
    fprintf ( fileout, "    txtsup_rot     0.000  0.000  0.000\n" );
    fprintf ( fileout, "    txtsup_trans   0.000  0.000  0.000\n" );
    fprintf ( fileout, "    txtsup_scal    1.000  1.000  1.000\n" );
    fprintf ( fileout, "  }\n" );

    text_num = text_num + 25;

  }
  fprintf ( fileout, "}\n" );
  text_num = text_num + 1;
//
//  Report.
//
  cout << "\n";
  cout << "HRC_WRITE - Wrote " << text_num << " text lines.\n";

  return 0;
}
//****************************************************************************80

int i4_max ( int i1, int i2 )

//****************************************************************************80
//
//  Purpose:
//
//    I4_MAX returns the maximum of two I4s.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    13 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int I1, I2, are two integers to be compared.
//
//    Output, int I4_MAX, the larger of I1 and I2.
//
{
  if ( i2 < i1 )
  {
    return i1;
  }
  else
  {
    return i2;
  }

}
//****************************************************************************80

int i4_min ( int i1, int i2 )

//****************************************************************************80
//
//  Purpose:
//
//    I4_MIN returns the smaller of two I4's.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    13 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int I1, I2, two integers to be compared.
//
//    Output, int I4_MIN, the smaller of I1 and I2.
//
{
  if ( i1 < i2 )
  {
    return i1;
  }
  else
  {
    return i2;
  }

}
//****************************************************************************80

int i4_modp ( int i, int j )

//****************************************************************************80
//
//  Purpose:
//
//    I4_MODP returns the nonnegative remainder of I4 division.
//
//  Discussion:
//
//    If
//      NREM = I4_MODP ( I, J )
//      NMULT = ( I - NREM ) / J
//    then
//      I = J * NMULT + NREM
//    where NREM is always nonnegative.
//
//    The MOD function computes a result with the same sign as the
//    quantity being divided.  Thus, suppose you had an angle A,
//    and you wanted to ensure that it was between 0 and 360.
//    Then mod(A,360) would do, if A was positive, but if A
//    was negative, your result would be between -360 and 0.
//
//    On the other hand, I4_MODP(A,360) is between 0 and 360, always.
//
//  Example:
//
//        I         J     MOD  I4_MODP   I4_MODP Factorization
//
//      107        50       7       7    107 =  2 *  50 + 7
//      107       -50       7       7    107 = -2 * -50 + 7
//     -107        50      -7      43   -107 = -3 *  50 + 43
//     -107       -50      -7      43   -107 =  3 * -50 + 43
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    26 May 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int I, the number to be divided.
//
//    Input, int J, the number that divides I.
//
//    Output, int I4_MODP, the nonnegative remainder when I is
//    divided by J.
//
{
  int value;

  if ( j == 0 )
  {
    cout << "\n";
    cout << "I4_MODP - Fatal error!\n";
    cout << "  I4_MODP ( I, J ) called with J = " << j << "\n";
    exit ( 1 );
  }

  value = i % j;

  if ( value < 0 )
  {
    value = value + abs ( j );
  }

  return value;
}
//****************************************************************************80

int i4_wrap ( int ival, int ilo, int ihi )

//****************************************************************************80
//
//  Purpose:
//
//    I4_WRAP forces an I4 to lie between given limits by wrapping.
//
//  Example:
//
//    ILO = 4, IHI = 8
//
//    I  I4_WRAP
//
//    -2     8
//    -1     4
//     0     5
//     1     6
//     2     7
//     3     8
//     4     4
//     5     5
//     6     6
//     7     7
//     8     8
//     9     4
//    10     5
//    11     6
//    12     7
//    13     8
//    14     4
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 August 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int IVAL, an integer value.
//
//    Input, int ILO, IHI, the desired bounds for the integer value.
//
//    Output, int I4_WRAP, a "wrapped" version of IVAL.
//
{
  int jhi;
  int jlo;
  int value;
  int wide;

  jlo = i4_min ( ilo, ihi );
  jhi = i4_max ( ilo, ihi );

  wide = jhi + 1 - jlo;

  if ( wide == 1 )
  {
    value = jlo;
  }
  else
  {
    value = jlo + i4_modp ( ival - jlo, wide );
  }

  return value;
}
//****************************************************************************80

void init_program_data ( )

//****************************************************************************80
//
//  Purpose:
//
//    INIT_PROGRAM_DATA initializes the internal program data.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    26 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  byte_swap = false;
  debug = false;
  line_prune = 1;
  color_num = 0;
  cor3_num = 0;
  face_num = 0;
  line_num = 0;

  if ( debug )
  {
    cout << "\n";
    cout << "INIT_PROGRAM_DATA: Program data initialized.\n";
  }

  return;

}
//****************************************************************************80

int interact ( )

//****************************************************************************80
//
//  Purpose:
//
//    INTERACT carries on an interactive session with the user.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    09 May 2005
//
//  Author:
//
//    John Burkardt
//
{
  int i;
  int icor3;
  int ierror;
  int iface;
  int itemp;
  int ivert;
  int j;
  int jvert;
  int m;
  char *next;
  bool success;
  float temp;
  float x;
  float y;
  float z;

  strcpy ( filein_name, "NO_IN_NAME" );
  strcpy ( fileout_name, "NO_OUT_NAME" );
//
//  Say hello.
//
  hello ( );
//
//  Get the next user command.
//
  cout << "\n";
  cout << "Enter command (H for help)\n";

  while ( fgets ( input, LINE_MAX_LEN, stdin ) != NULL )
  {
//
//  Advance to the first nonspace character in INPUT.
//
    for ( next = input; *next != '\0' && ch_is_space ( *next ); next++ )
    {
    }
//
//  Skip blank lines and comments.
//
    if ( *next == '\0' )
    {
      continue;
    }
//
//  Command: << FILENAME
//  Append new data to current graphics information.
//
    if ( *next == '<' && *(next+1) == '<' )
    {
      next = next + 2;
      sscanf ( next, "%s", filein_name );

      success = data_read ( );

      if ( !success )
      {
        cout << "\n";
        cout << "INTERACT - Fatal error!\n";
        cout << "  Failure reported from DATA_READ.\n";
      }
    }
//
//  Command: < FILENAME
//
    else if ( *next == '<' )
    {
      next = next + 1;
      sscanf ( next, "%s", filein_name );

      data_init ( );

      success = data_read ( );

      if ( !success )
      {
        cout << "\n";
        cout << "INTERACT - Fatal error!\n";
        cout << "  DATA_READ failed to read input data.\n";
      }
    }
//
//  Command: > FILENAME
//
    else if ( *next == '>' )
    {
      next = next + 1;
      sscanf ( next, "%s", fileout_name );

      ierror = data_write ( );

      if ( ierror == ERROR )
      {
        cout << "\n";
        cout << "INTERACT - Fatal error!\n";
        cout << "  OUTPUT_DATA failed to write output data.\n";
      }

    }
//
//  B: Switch byte swapping option.
//
    else if ( *next == 'B' || *next == 'b' )
    {
      if ( byte_swap )
      {
        byte_swap = false;
        cout << "Byte_swapping reset to FALSE.\n";
      }
      else
      {
        byte_swap = true;
        cout << "Byte_swapping reset to TRUE.\n";
      }

    }
//
//  D: Switch debug option.
//
    else if ( *next == 'D' || *next == 'd' )
    {
      if ( debug )
      {
        debug = false;
        cout << "Debug reset to FALSE.\n";
      }
      else
      {
        debug = true;
        cout << "Debug reset to TRUE.\n";
      }
    }
//
//  F: Check a face.
//
    else if ( *next == 'f' || *next == 'F' )
    {
      cout << "\n";
      cout << "  Enter a face index between 0 and " << face_num-1 << "\n";
      scanf ( "%d", &iface );
      face_print ( iface );
    }
//
//  H: Help
//
    else if ( *next == 'h' || *next == 'H' )
    {
      help ( );
    }
//
//  I: Print change information.
//
    else if ( *next == 'i' || *next == 'I')
    {
      news ( );
    }
//
//  LINES:
//  Convert face information to lines.
//
    else if ( *next == 'l' || *next == 'L')
    {
      if ( 0 < face_num )
      {
        cout << "\n";
        cout << "INTERACT - Note:\n";
        cout << "  Face information will be converted\n";
        cout << "  to line information.\n";

          face_to_line ( );

          if ( LINES_MAX < line_num )
          {
            cout << "\n";
            cout << "INTERACT - Note:\n";
            cout << "  Some face information was lost.\n";
            cout << "  The maximum number of lines is " << LINES_MAX << "\n";
            cout << "  but we would need at least " << line_num << ".\n";

            line_num = LINES_MAX;

          }

          face_num = 0;
      }
      else
      {
        cout << "\n";
        cout << "INTERACT - Note:\n";
        cout << "  There were no faces to convert.\n";

      }

    }
//
//  N: Recompute normal vectors.
//
    else if ( *next == 'n' || *next == 'N' )
    {
      for ( iface = 0; iface < face_num; iface++ )
      {
        for ( i = 0; i < 3; i++ )
        {
          face_normal[i][iface] = 0.0;
        }
      }

      for ( iface = 0; iface < face_num; iface++ )
      {
         for ( ivert = 0; ivert < face_order[iface]; ivert++ )
         {
          for ( i = 0; i < 3; i++ )
          {
            vertex_normal[i][ivert][iface] = 0.0;
          }
        }
      }

      vertex_normal_set ( );

      cor3_normal_set ( );

      face_normal_ave ( );
    }
//
//  P: Line pruning optiont
//
    else if ( *next == 'p' || *next == 'P' )
    {
      cout << "\n";
      cout << "INTERACT - SET LINE PRUNING OPTION.\n";
      cout << "\n";
      cout << "  LINE_PRUNE = 0 means no line pruning.\n";
      cout << "               nonzero means line pruning.\n";
      cout << "\n";
      cout << "  Current value is LINE_PRUNE = " << line_prune << ".\n";
      cout << "\n";
      cout << "  Enter new value for LINE_PRUNE.\n";

      if ( fgets ( input, LINE_MAX_LEN, stdin ) == NULL )
      {
        cout << "  ??? Error trying to read input.\n" ;
      }
      else
      {
        sscanf ( input, "%d", &line_prune );
        cout << "  New value is LINE_PRUNE = " << line_prune << ".\n";
      }
    }
//
//  Q: Quit
//
    else if ( *next == 'q' || *next == 'Q' )
    {
      cout << "\n";
      cout << "INTERACT - Normal end of execution.\n";
      return 0;
    }
//
//  R: Reverse normal vectors.
//
    else if ( *next == 'r' || *next == 'R' )
    {
      for ( icor3 = 0; icor3 < cor3_num; icor3++ )
      {
        for ( i = 0; i < 3; i++ )
        {
          cor3_normal[i][icor3] = - cor3_normal[i][icor3];
        }
      }

      for ( iface = 0; iface < face_num; iface++ )
      {
        for ( i = 0; i < 3; i++ )
        {
          face_normal[i][iface] = - face_normal[i][iface];
        }
      }

      for ( iface = 0; iface < face_num; iface++ )
      {
        for ( ivert = 0; ivert < face_order[iface]; ivert++ )
        {
          for ( i = 0; i < 3; i++ )
          {
            vertex_normal[i][ivert][iface] =
              - vertex_normal[i][ivert][iface];
          }
        }
      }
      cout << "\n";
      cout << "INTERACT - Note:\n";
      cout << "  Reversed node, face and vertex normals.\n";
    }
//
//  S: Select a few faces, discard the rest.
//
    else if ( *next == 's' || *next == 'S' )
    {
      face_subset ( );
    }
//
//  T: Transform the data.
//
    else if ( *next == 't' || *next == 'T' )
    {
      cout << "\n";
      cout << "For now, we only offer point scaling.\n";
      cout << "Enter X, Y, Z scale factors:\n";

      scanf ( "%f %f %f", &x, &y, &z );

      for ( j = 0; j < cor3_num; j++ )
      {
        cor3[0][j] = x * cor3[0][j];
        cor3[1][j] = y * cor3[1][j];
        cor3[2][j] = z * cor3[2][j];
      }

      for ( iface = 0; iface < face_num; iface++ )
      {
        for ( i = 0; i < 3; i++ )
        {
          face_normal[i][iface] = 0.0;
        }
      }

      for ( iface = 0; iface < face_num; iface++ )
      {
         for ( ivert = 0; ivert < face_order[iface]; ivert++ )
         {
          for ( i = 0; i < 3; i++ )
          {
            vertex_normal[i][ivert][iface] = 0.0;
          }
        }
      }

      vertex_normal_set ( );

      cor3_normal_set ( );

      face_normal_ave ( );
    }
//
//  U: Renumber faces, count objects:
//
    else if ( *next == 'u' || *next == 'U' )
    {
    }
//
//  V: Convert polygons to triangles:
//
    else if ( *next == 'v' || *next == 'V' )
    {
    }
//
//  W: Reverse the face node ordering.
//
    else if ( *next == 'w' || *next == 'W' )
    {
      if ( 0 < face_num )
      {
        for ( iface = 0; iface < face_num; iface++ )
        {
          m = face_order[iface];

          for ( ivert = 0; ivert < m/2; ivert++ )
          {
            jvert = m - ivert - 1;

            itemp = face[ivert][iface];
            face[ivert][iface] = face[jvert][iface];
            face[jvert][iface] = itemp;

            itemp = vertex_material[ivert][iface];
            vertex_material[ivert][iface] = vertex_material[jvert][iface];
            vertex_material[jvert][iface] = itemp;

            for ( i = 0; i < 3; i++ )
            {
              temp = vertex_normal[i][ivert][iface];
              vertex_normal[i][ivert][iface] =
                vertex_normal[i][jvert][iface];
              vertex_normal[i][jvert][iface] = temp;
            }
          }
        }
        cout << "\n";
        cout << "INTERACT - Note:\n";
        cout << "  Reversed face node ordering.\n";
      }
    }
//
//  Command: ???
//
    else
    {
      cout << "\n";
      cout << "INTERACT: Warning!\n";
      cout << "  Your command was not recognized.\n";
    }

    cout << "\n";
    cout << "Enter command (H for help)\n";

  }
  return 0;
}
//****************************************************************************80

int iv_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    IV_READ reads graphics information from an Inventor file.
//
//  Example:
//
//     #Inventor V2.0 ascii
//
//     Separator {
//       Info {
//         string "Inventor file generated by IVCON.
//         Original data in file cube.iv."
//       }
//       Separator {
//         LightModel {
//           model PHONG
//         }
//         MatrixTransform { matrix
//           0.9  0.0  0.0  0.0
//           0.0 -0.9  0.0  0.0
//           0.0  0.0 -1.5  0.0
//           0.0  0.0  0.0  1.0
//         }
//         Material {
//           ambientColor  0.2 0.2 0.2
//           diffuseColor  [
//             0.8 0.8 0.8,
//             0.7 0.1 0.1,
//             0.1 0.8 0.2,
//           ]
//           emissiveColor 0.0 0.0 0.0
//           specularColor 0.0 0.0 0.0
//           shininess     0.2
//           transparency  [
//             0.0, 0.5, 1.0,
//           ]
//         }
//         Texture2 {
//           filename      "fred.rgb"
//           wrapS         REPEAT
//           wrapT         REPEAT
//           model         MODULATE
//           blendColor    0.0 0.0 0.0
//         }
//
//         MaterialBinding {
//           value PER_VERTEX_INDEXED
//         }
//         NormalBinding {
//           value PER_VERTEX_INDEXED
//         }
//         TextureCoordinateBinding {
//           value PER_VERTEX_INDEXED
//         }
//
//         ShapeHints {
//           vertexOrdering COUNTERCLOCKWISE
//           shapeType UNKNOWN_SHAPE_TYPE
//           faceType CONVEX
//           creaseAngle 6.28319
//         }
//
//         Coordinate3 {
//           point [
//                8.59816       5.55317      -3.05561,
//                8.59816       2.49756      0.000000E+00,
//                ...etc...
//                2.48695       2.49756      -3.05561,
//           ]
//         }
//
//         Normal {
//           vector [
//             0.71 0.71 0.0,
//             ...etc...
//             0.32 0.32 0.41,
//           ]
//         }
//
//         TextureCoordinate2 {
//           point [
//                0.0  1.0,
//                0.1, 0.8,
//                ...etc...
//                0.4  0.7,
//           ]
//         }
//
//         IndexedLineSet {
//           coordIndex [
//              0,    1,    2,   -1,
//              3,    4,    5,   -1,
//              7,    8,    9,   -1,
//            ...etc...
//            189,  190,  191,   -1,
//           ]
//           materialIndex [
//              0,    0,    0,   -1,
//              1,    1,    1,   -1,
//              2,    2,    2,   -1,
//            ...etc...
//             64,   64,   64,   -1,
//           ]
//         }
//
//         IndexedFaceSet {
//           coordIndex [
//              0,    1,    2,   -1,
//              3,    4,    5,   -1,
//              7,    8,    9,   -1,
//            ...etc...
//            189,  190,  191,   -1,
//           ]
//           materialIndex [
//              0,    0,    0,   -1,
//              1,    1,    1,   -1,
//              2,    2,    2,   -1,
//            ...etc...
//             64,   64,   64,   -1,
//           ]
//           normalIndex [
//              0,    0,    0,   -1,
//              1,    1,    1,   -1,
//              2,    2,    2,   -1,
//            ...etc...
//             64,   64,   64,   -1,
//           ]
//           textureCoordIndex [
//              0,    0,    0,   -1,
//              1,    1,    1,   -1,
//              2,    2,    2,   -1,
//            ...etc...
//             64,   64,   64,   -1,
//           ]
//         }
//
//         IndexedTriangleStripSet {
//           vertexProperty VertexProperty {
//             vertex [ x y z,
//                      ...
//                      x y z ]
//             normal [ x y z,
//                      ...
//                      x y z ]
//             materialBinding OVERALL
//             normalBinding PER_VERTEX_INDEXED
//           }
//           coordIndex [
//             i, j, k, l, m, -1,
//             n, o, p, q, r, s, t, u, -1,
//             v, w, x, -1
//                 ..., -1 ]
//           normalIndex -1
//         }
//
//       }
//     }
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    01 July 1999
//
//  Author:
//
//    John Burkardt
//
{
  char  c;
  int   count;
  int   i;
  int   icol;
  int   icolor;
  int   icface;
  int   inormface;
  int   iface_num;
  int   irow;
  int   iuv;
  int   ivert;
  int   iword;
  int   ix;
  int   ixyz;
  int   iy;
  int   iz;
  int   j;
  int   jval;
  int   level;
  char *next;
  int   nlbrack;
  int   nrbrack;
  int   nu;
  int   null_index;
  int   cor3_num_old;
  int   line_num2;
  int   face_num2;
  int   normal_num_temp;
  int   text_numure_temp;
  int   nv;
  int   result;
  float rval;
  int   width;
  char  word[LINE_MAX_LEN];
  char  word1[LINE_MAX_LEN];
  char  wordm1[LINE_MAX_LEN];
  float xvec[3];

  icface = 0;
  icol = -1;
  inormface = 0;
  iface_num = face_num;
  irow = 0;
  ix = 0;
  ixyz = 0;
  iy = 0;
  iz = 0;
  jval = 0;
  level = 0;
  strcpy ( level_name[0], "Top" );
  nlbrack = 0;
  nrbrack = 0;
  nu = 0;
  cor3_num_old = cor3_num;
  face_num2 = face_num;
  line_num2 = line_num;
  normal_num_temp = 0;
  text_numure_temp = 0;
  nv = 0;
  rval = 0.0;
  strcpy ( word, " " );
  strcpy ( wordm1, " " );
//
//  Read the next line of text from the input file.
//
  for ( ;; )
  {
    if ( fgets ( input, LINE_MAX_LEN, filein ) == NULL )
    {
      break;
    }

    text_num = text_num + 1;
    next = input;
    iword = 0;
//
//  Remove all commas from the line, so we can use SSCANF to read
//  numeric items.
//
    i = 0;
    while ( input[i] != '\0' )
    {
      if ( input[i] == ',' )
      {
        input[i] = ' ';
      }
      i++;
    }
//
//  Force brackets and braces to be buffered by spaces.
//
    i = 0;
    while ( input[i] != '\0' )
    {
      i++;
    }
    null_index = i;

    i = 0;
    while ( input[i] != '\0' && i < LINE_MAX_LEN )
    {
      if ( input[i] == '[' || input[i] == ']' ||
           input[i] == '{' || input[i] == '}' )
      {
        result = ch_pad ( &i, &null_index, input, LINE_MAX_LEN );
        if ( result == ERROR )
        {
          break;
        }
      }
      else
      {
        i++;
      }
    }
//
//  Read a word from the line.
//
    for ( ;; )
    {
      strcpy ( wordm1, word );
      strcpy ( word, " " );

      count = sscanf ( next, "%s%n", word, &width );
      next = next + width;

      if ( count <= 0 )
      {
        break;
      }

      iword = iword + 1;

      if ( iword == 1 )
      {
        strcpy ( word1, word );
      }
//
//  The first line of the file must be the header.
//
      if ( text_num == 1 )
      {
        if ( !s_eqi ( word1, "#Inventor" ) )
        {
          cout << "\n";
          cout << "IV_READ - Fatal error!\n";
          cout << "  The input file has a bad header.\n";
          return 1;
        }
        else
        {
          comment_num = comment_num + 1;
        }
        break;
      }
//
//  A comment begins anywhere with '#'.
//  Skip the rest of the line.
//
      if ( word[1] == '#' )
      {
        comment_num = comment_num + 1;
        break;
      }
//
//  If the word is a curly or square bracket, count it.
//  If the word is a left bracket, the previous word is the name of a node.
//
      if ( strcmp ( word, "{" ) == 0 || strcmp ( word, "[" ) == 0 )
      {
        nlbrack = nlbrack + 1;
        level = nlbrack - nrbrack;
        strcpy ( level_name[level], wordm1 );
        if ( debug )
        {
          cout << "Begin level: " << wordm1 << "\n";
        }
      }
      else if ( strcmp ( word, "}" ) == 0 || strcmp ( word, "]" ) == 0 )
      {
        nrbrack = nrbrack + 1;

        if ( nlbrack < nrbrack )
        {
          cout << "\n";
          cout << "IV_READ - Fatal error!\n";
          cout << "  Extraneous right bracket on line " << text_num << ".\n";
          cout << "  Currently processing field " << level_name[level] << "\n";
          return 1;
        }
      }
//
//  BASECOLOR
//
      if ( s_eqi ( level_name[level], "BASECOLOR" ) )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( s_eqi ( word, "RGB" ) )
        {
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "Bad data " << word << "\n";
        }
      }
//
//  COORDINATE3
//
      else if ( s_eqi ( level_name[level], "COORDINATE3" ) )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( s_eqi ( word, "POINT" ) )
        {
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "COORDINATE3: Bad data " << word << "\n";
        }
      }
//
//  COORDINATE4
//
      else if ( s_eqi ( level_name[level], "COORDINATE4" ) )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( s_eqi ( word, "POINT" ) )
        {
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "COORDINATE4: Bad data " << word << "\n";
        }
      }
//
//  COORDINDEX
//
      else if ( s_eqi ( level_name[level], "COORDINDEX" ) )
      {
        if ( strcmp ( word, "[" ) == 0 )
        {
          ivert = 0;
        }
        else if ( strcmp ( word, "]" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
//
//  (indexedlineset) COORDINDEX
//
        else if ( s_eqi ( level_name[level-1], "INDEXEDLINESET" ) )
        {

          count = sscanf ( word, "%d%n", &jval, &width );

          if ( 0 < count )
          {
            if ( jval < -1 )
            {
              bad_num = bad_num + 1;
            }
            else
            {
              if ( line_num < LINES_MAX )
              {
                if ( jval != -1 )
                {
                  jval = jval + cor3_num_old;
                }
                line_dex[line_num] = jval;
              }
              line_num = line_num + 1;
            }
          }
          else
          {
            bad_num = bad_num + 1;
          }
        }
//
//  (indexedfaceset) COORDINDEX
//  Warning: If the list of indices is not terminated with a final -1, then
//  the last face won't get counted.
//
        else if ( s_eqi ( level_name[level-1], "INDEXEDFACESET" ) )
        {
          count = sscanf ( word, "%d%n", &jval, &width );

          if ( 0 < count )
          {
            if ( jval == -1 )
            {
              ivert = 0;
              face_num = face_num + 1;
            }
            else
            {
              if ( ivert == 0 )
              {
                if ( face_num < FACE_MAX )
                {
                  face_order[face_num] = 0;
                }
              }
              if ( face_num < FACE_MAX )
              {
                face_order[face_num] = face_order[face_num] + 1;
                face[ivert][face_num] = jval + cor3_num_old;
                ivert = ivert + 1;
              }
            }
          }
        }
//
//  (indexednurbssurface) COORDINDEX
//
        else if ( s_eqi ( level_name[level-1], "INDEXEDNURBSSURFACE" ) )
        {
        }
//
//  (indexedtrianglestripset) COORDINDEX
//
//  First three coordinate indices I1, I2, I3 define a triangle.
//  Next triangle is defined by I2, I3, I4 (actually, I4, I3, I2
//  to stay with same counterclockwise sense).
//  Next triangle is defined by I3, I4, I5 ( do not need to reverse
//  odd numbered triangles) and so on.
//  List is terminated with -1.
//
        else if ( s_eqi ( level_name[level-1], "INDEXEDTRIANGLESTRIPSET" ) )
        {
          count = sscanf ( word, "%d%n", &jval, &width );

          if ( 0 < count )
          {
            if ( jval == -1 )
            {
              ivert = 0;
            }
            else
            {
              ix = iy;
              iy = iz;
              iz = jval + cor3_num_old;

              if ( ivert == 0 )
              {
                if ( face_num < FACE_MAX )
                {
                  face[ivert][face_num] = jval + cor3_num_old;
                  face_order[face_num] = 3;
                }
              }
              else if ( ivert == 1 )
              {
                if ( face_num < FACE_MAX )
                {
                  face[ivert][face_num] = jval + cor3_num_old;
                }
              }
              else if ( ivert == 2 )
              {
                if ( face_num < FACE_MAX )
                {
                  face[ivert][face_num] = jval + cor3_num_old;
                }
                face_num = face_num + 1;
              }
              else
              {
                if ( face_num < FACE_MAX )
                {
                  face_order[face_num] = 3;
                  if ( ( ivert % 2 ) == 0 )
                  {
                    face[0][face_num] = ix;
                    face[1][face_num] = iy;
                    face[2][face_num] = iz;
                  }
                  else
                  {
                    face[0][face_num] = iz;
                    face[1][face_num] = iy;
                    face[2][face_num] = ix;
                  }
                }
                face_num = face_num + 1;
              }
              ivert = ivert + 1;
//
//  Very very tentative guess as to how indices into the normal
//  vector array are set up...
//
              if ( face_num < FACE_MAX && 2 < ivert )
              {
                for ( i = 0; i < 3; i++ )
                {
                  face_normal[i][face_num] = normal_temp[i][ix];
                }
              }
            }
          }
        }
      }
//
//  INDEXEDFACESET
//
      else if ( s_eqi ( level_name[level], "INDEXEDFACESET" ) )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( s_eqi ( word, "COORDINDEX" ) )
        {
          ivert = 0;
        }
        else if ( s_eqi ( word, "MATERIALINDEX" ) )
        {
        }
        else if ( s_eqi ( word, "NORMALINDEX" ) )
        {
        }
        else if ( s_eqi ( word, "TEXTURECOORDINDEX" ) )
        {
          if ( texture_num <= 0 )
          {
            texture_num = 1;
            strcpy ( texture_name[0], "Texture_0000" );
          }
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "Bad data " << word << "\n";
        }
      }
//
//  INDEXEDLINESET
//
      else if ( s_eqi ( level_name[level], "INDEXEDLINESET" ) )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( s_eqi ( word, "COORDINDEX" ) )
        {
        }
        else if ( s_eqi ( word, "MATERIALINDEX" ) )
        {
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "Bad data " << word << "\n";
        }
      }
//
//  INDEXEDNURBSSURFACE
//
      else if ( s_eqi ( level_name[level], "INDEXEDNURBSSURFACE" ) )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( s_eqi ( word, "NUMUCONTROLPOINTS" ) )
        {
          count = sscanf ( word, "%d%n", &jval, &width );

          if ( 0 < count )
          {
            nu = jval;
            if ( debug )
            {
              cout << "NU = " << nu << "\n";
            }
          }
          else
          {
            nu = 0;
            bad_num = bad_num + 1;
            cout << "Bad data " << word << "\n";
          }
        }
        else if ( s_eqi ( word, "NUMVCONTROLPOINTS" ) )
        {
          count = sscanf ( word, "%d%n", &jval, &width );

          if ( 0 < count )
          {
            nv = jval;
            if ( debug )
            {
              cout << "NV = " << nv << "\n";
            }
          }
          else
          {
            nv = 0;
            bad_num = bad_num + 1;
          }
        }
        else if ( s_eqi ( word, "COORDINDEX" ) )
        {
        }
        else if ( s_eqi ( word, "UKNOTVECTOR" ) )
        {
        }
        else if ( s_eqi ( word, "VKNOTVECTOR" ) )
        {
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "Bad data " << word << "\n";
        }
      }
//
//  INDEXEDTRIANGLESTRIPSET
//
      else if ( s_eqi ( level_name[level], "INDEXEDTRIANGLESTRIPSET" ) )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( s_eqi ( word, "VERTEXPROPERTY" ) )
        {
          count = sscanf ( next, "%s%n", word, &width );
          next = next + width;
        }
        else if ( s_eqi ( word, "COORDINDEX" ) )
        {
          ivert = 0;
        }
        else if ( s_eqi ( word, "NORMALINDEX" ) )
        {
          count = sscanf ( next, "%s%n", word, &width );
          next = next + width;
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "Bad data " << word << "\n";
        }
      }
//
//  INFO
//
      else if ( s_eqi ( level_name[level], "INFO" ) )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( s_eqi ( word, "STRING" ) )
        {
        }
        else if ( strcmp ( word, "\"" ) == 0 )
        {
        }
        else
        {
        }
      }
//
//  LIGHTMODEL
//  Read, but ignore.
//
      else if ( s_eqi ( level_name[level], "LIGHTMODEL" ) )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( s_eqi ( word, "model" ) )
        {
        }
        else
        {
        }
      }
//
//  MATERIAL
//  Read, but ignore.
//
      else if ( s_eqi ( level_name[level],"MATERIAL" ) )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( s_eqi ( word, "AMBIENTCOLOR" ) )
        {
        }
        else if ( s_eqi ( word, "EMISSIVECOLOR" ) )
        {
        }
        else if ( s_eqi ( word, "DIFFUSECOLOR" ) )
        {
        }
        else if ( s_eqi ( word, "SHININESS" ) )
        {
        }
        else if ( s_eqi ( word, "SPECULARCOLOR" ) )
        {
        }
        else if ( s_eqi ( word, "TRANSPARENCY" ) )
        {
        }
        else
        {
        }
      }
//
//  MATERIALBINDING
//  Read, but ignore
//
      else if ( s_eqi ( level_name[level], "MATERIALBINDING" ) )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( s_eqi ( word, "VALUE" ) )
        {
          count = sscanf ( next, "%s%n", material_binding, &width );
          next = next + width;
        }
        else
        {
          count = sscanf ( next, "%f%n", &rval, &width );
          next = next + width;

          if ( 0 < count )
          {
          }
          else
          {
            bad_num = bad_num + 1;
            cout << "Bad data " << word << "\n";
          }
        }
      }
//
//  MATERIALINDEX
//
      else if ( s_eqi ( level_name[level], "MATERIALINDEX" ) )
      {
        if ( strcmp ( word, "[" ) == 0 )
        {
          ivert = 0;
        }
        else if ( strcmp ( word, "]" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
//
//  (indexedfaceset) MATERIALINDEX
//
        else if ( s_eqi ( level_name[level-1], "INDEXEDFACESET" ) )
        {
          count = sscanf ( word, "%d%n", &jval, &width );

          if ( 0 < count )
          {
            if ( jval == -1 )
            {
              ivert = 0;
              face_num2 = face_num2 + 1;
            }
            else
            {
              if ( face_num2 < FACE_MAX )
              {
                if ( jval != -1 )
                {
                  jval = jval + cor3_num_old;
                }
                vertex_material[ivert][face_num2] = jval;
                ivert = ivert + 1;
              }
            }
          }
          else
          {
            bad_num = bad_num + 1;
            cout << "Bad data " << word << "\n";
          }
        }
//
//  (indexedlineset) MATERIALINDEX
//
        else if ( s_eqi ( level_name[level-1], "INDEXEDLINESET" ) )
        {
          count = sscanf ( word, "%d%n", &jval, &width );

          if ( 0 < count )
          {
            if ( line_num2 < LINES_MAX )
            {
              if ( jval != -1 )
              {
                jval = jval + cor3_num_old;
              }
              line_material[line_num2] = jval;
              line_num2 = line_num2 + 1;
            }
          }
          else
          {
            bad_num = bad_num + 1;
            cout << "Bad data " << word << "\n";
          }
        }
        else
        {
          count = sscanf ( word, "%d%n", &jval, &width );

          if ( 0 < count )
          {
          }
          else
          {
            bad_num = bad_num + 1;
            cout << "Bad data " << word << "\n";
          }
        }
      }
//
//  MATRIXTRANSFORM.
//
      else if ( s_eqi ( level_name[level], "MATRIXTRANSFORM" ) )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( s_eqi ( word, "MATRIX" ) )
        {
          icol = -1;
          irow = 0;
        }
        else
        {
          count = sscanf ( word, "%f%n", &rval, &width );

          if ( 0 < count )
          {
            icol = icol + 1;
            if ( 3 < icol )
            {
              icol = 0;
              irow = irow + 1;
              if ( 3 < irow )
              {
                irow = 0;
              }
            }

            transform_matrix[irow][icol] = rval;
          }

        }
      }
//
//  NORMAL
//  The field "VECTOR" may be followed by three numbers,
//  (handled here),  or by a square bracket, and sets of three numbers.
//
      else if ( s_eqi ( level_name[level], "NORMAL" ) )
      {
//
//  (vertexproperty) NORMAL
//
        if ( s_eqi ( level_name[level-1], "VERTEXPROPERTY" ) )
        {
          if ( strcmp ( word, "[" ) == 0 )
          {
            ixyz = 0;
          }
          else if ( strcmp ( word, "]" ) == 0 )
          {
            level = nlbrack - nrbrack;
          }
          else
          {

            count = sscanf ( word, "%f%n", &rval, &width );

            if ( 0 < count )
            {

              if ( inormface < FACE_MAX )
              {
                face_normal[ixyz][inormface] = rval;
              }

              ixyz = ixyz + 1;
              if ( 2 < ixyz )
              {
                ixyz = 0;
                inormface = inormface + 1;
              }
            }
          }
        }
//
//  (anythingelse) NORMAL
//
        else
        {
          if ( strcmp ( word, "{" ) == 0 )
          {
            ixyz = 0;
          }
          else if ( strcmp ( word, "}" ) == 0 )
          {
            level = nlbrack - nrbrack;
          }
          else if ( s_eqi ( word, "VECTOR" ) )
          {
          }
          else
          {
            count = sscanf ( word, "%f%n", &rval, &width );

            if ( 0 < count )
            {

//
//  COMMENTED OUT!
//
//              if ( nfnorm < FACE_MAX )
//              {
//                normal[ixyz][nfnorm] = rval;
//              }
//
//
              ixyz = ixyz + 1;
              if ( 2 < ixyz )
              {
                ixyz = 0;
              }
            }
            else
            {
              bad_num = bad_num + 1;
              cout << "Bad data " << word << "\n";
            }
          }
        }
      }
//
//  NORMALBINDING
//  Read, but ignore
//
      else if ( s_eqi ( level_name[level], "NORMALBINDING" ) )
      {

        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( s_eqi ( word, "VALUE" ) )
        {
          count = sscanf ( next, "%s%n", normal_binding, &width );
          next = next + width;
        }
        else
        {
          count = sscanf ( word, "%f%n", &rval, &width );

          if ( 0 < count )
          {
          }
          else
          {
            bad_num = bad_num + 1;
            cout << "Bad data " << word << "\n";
          }
        }
      }
//
//  NORMALINDEX
//
      else if ( s_eqi ( level_name[level], "NORMALINDEX" ) )
      {
//
//  (indexedtrianglestripset) NORMALINDEX
//
        if ( s_eqi ( level_name[level-1], "INDEXEDTRIANGLESTRIPSET" ) )
        {
          count = sscanf ( word, "%d%n", &jval, &width );

          if ( 0 < count )
          {
          }
          else if ( strcmp ( word, "[" ) == 0 )
          {
          }
          else if ( strcmp ( word, "]" ) == 0 )
          {
          }
        }
//
//  (anythingelse) NORMALINDEX
//
        else
        {

          if ( strcmp ( word, "[" ) == 0 )
          {
            ivert = 0;
          }
          else if ( strcmp ( word, "]" ) == 0 )
          {
            level = nlbrack - nrbrack;
          }
          else
          {
            count = sscanf ( word, "%d%n", &jval, &width );

            if ( 0 < count )
            {
              if ( jval == -1 )
              {
                ivert = 0;
                iface_num = iface_num + 1;
              }
              else
              {
                if ( iface_num < FACE_MAX )
                {
                  for ( i = 0; i < 3; i++ )
                  {
                    vertex_normal[i][ivert][iface_num] = normal_temp[i][jval];
                  }
                  ivert = ivert + 1;
                }
              }
            }
            else
            {
              bad_num = bad_num + 1;
              cout << "Bad data " << word << "\n";
            }
          }
        }
      }
//
//  (coordinate3) POINT
//
      else if ( s_eqi ( level_name[level], "POINT" ) )
      {
        if ( s_eqi ( level_name[level-1], "COORDINATE3" ) )
        {
          if ( strcmp ( word, "[" ) == 0 )
          {
            ixyz = 0;
            cor3_num_old = cor3_num;
          }
          else if ( strcmp ( word, "]" ) == 0 )
          {
            level = nlbrack - nrbrack;
          }
          else
          {
            count = sscanf ( word, "%f%n", &rval, &width );

            if ( 0 < count )
            {
              if ( cor3_num < COR3_MAX )
              {
                xvec[ixyz] = rval;
              }

              ixyz = ixyz + 1;

              if ( ixyz == 3 )
              {
                ixyz = 0;

                tmat_mxp ( transform_matrix, xvec, xvec );

                cor3[0][cor3_num] = xvec[0];
                cor3[1][cor3_num] = xvec[1];
                cor3[2][cor3_num] = xvec[2];

                cor3_num = cor3_num + 1;

                continue;
              }
            }
            else
            {
              bad_num = bad_num + 1;
              break;
            }
          }
        }
//
//  (texturecoodinate2) POINT
//
        else if ( s_eqi ( level_name[level-1], "TEXTURECOORDINATE2" ) )
        {
          if ( strcmp ( word, "[" ) == 0 )
          {
            iuv = 0;
            text_numure_temp = 0;
          }
          else if ( strcmp ( word, "]" ) == 0 )
          {
            level = nlbrack - nrbrack;
          }
          else
          {

            count = sscanf ( word, "%f%n", &rval, &width );

            if ( 0 < count )
            {

              texture_temp[iuv][text_numure_temp] = rval;

              iuv = iuv + 1;
              if ( iuv == 2 )
              {
                iuv = 0;
                text_numure_temp = text_numure_temp + 1;
              }
            }
            else
            {
              cout << "TextureCoordinate2 { Point [: Bad data\n";
              bad_num = bad_num + 1;
              break;
            }
          }
        }
      }
//
//  RGB
//
      else if ( s_eqi ( level_name[level],"RGB" ) )
      {
//
//  (basecolor) RGB
//
        if ( s_eqi ( level_name[level-1], "BASECOLOR" ) )
        {
          if ( strcmp ( word, "[" ) == 0 )
          {
            icolor = 0;
          }
          else if ( strcmp ( word, "]" ) == 0 )
          {
            level = nlbrack - nrbrack;
          }
          else
          {
            count = sscanf ( word, "%f%n", &rval, &width );

            if ( 0 < count )
            {

              rgbcolor[icolor][color_num] = rval;
              icolor = icolor + 1;

              if ( icolor == 3 )
              {
                icolor = 0;
                color_num = color_num + 1;
              }
            }
            else
            {
              bad_num = bad_num + 1;
              cout << "Bad data " << word << "\n";
            }
          }
        }
//
//  (anythingelse RGB)
//
        else
        {
          cout << "HALSBAND DES TODES!\n";

          if ( strcmp ( word, "[" ) == 0 )
          {
            icolor = 0;
            ivert = 0;
          }
          else if ( strcmp ( word, "]" ) == 0 )
          {
            level = nlbrack - nrbrack;
          }
          else
          {

            count = sscanf ( word, "%f%n", &rval, &width );

            if ( 0 < count )
            {

              if ( icface < FACE_MAX )
              {

                vertex_rgb[icolor][ivert][icface] = rval;

                icolor = icolor + 1;
                if ( icolor == 3 )
                {
                  icolor = 0;
                  color_num = color_num + 1;
                  ivert = ivert + 1;
                  if ( ivert == face_order[icface] )
                  {
                    ivert = 0;
                    icface = icface + 1;
                  }
                }
              }
            }
            else
            {
              bad_num = bad_num + 1;
              cout << "Bad data " << word << "\n";
            }
          }
        }

      }
//
//  SEPARATOR
//
      else if ( s_eqi ( level_name[level], "SEPARATOR" ) )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else
        {
        }
      }
//
//  SHAPEHINTS
//  Read, but ignore.
//
      else if ( s_eqi ( level_name[level], "SHAPEHINTS" ) )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( s_eqi ( word, "CREASEANGLE" ) )
        {
          count = sscanf ( next, "%f%n", &rval, &width );
          next = next + width;

          if ( count <= 0 )
          {
            bad_num = bad_num + 1;
            cout << "Bad data " << word << "\n";
          }
        }
        else if ( s_eqi ( word, "FACETYPE" ) )
        {
          count = sscanf ( next, "%s%n", word, &width );
          next = next + width;
        }
        else if ( s_eqi ( word, "SHAPETYPE" ) )
        {
          count = sscanf ( next, "%s%n", word, &width );
          next = next + width;
        }
        else if ( s_eqi ( word, "VERTEXORDERING" ) )
        {
          count = sscanf ( next, "%s%n", word, &width );
          next = next + width;
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "Bad data " << word << "\n";
        }
      }
//
//  TEXTURE2
//
      else if ( s_eqi ( level_name[level], "TEXTURE2" ) )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
          texture_num = texture_num + 1;
        }
        else if ( s_eqi ( word, "BLENDCOLOR" ) )
        {
        }
//
//  NEED TO REMOVE QUOTES SURROUNDING TEXTURE NAME.
//
        else if ( s_eqi ( word, "FILENAME" ) )
        {
          count = sscanf ( next, "%s%n", word, &width );
          next = next + width;

          strcpy ( texture_name[texture_num], word );

          i = 0;
          j = 0;
          do
          {
            c = texture_name[texture_num][i];
            i = i + 1;
            if ( c != '"' )
            {
              texture_name[texture_num][j] = c;
              j = j + 1;
            }
          } while ( c != '\0' );

        }
        else if ( s_eqi ( word, "IMAGE" ) )
        {
        }
        else if ( s_eqi ( word, "MODEL" ) )
        {
          count = sscanf ( next, "%s%n", word, &width );
          next = next + width;
        }
        else if ( s_eqi ( word, "WRAPS" ) )
        {
          count = sscanf ( next, "%s%n", word, &width );
          next = next + width;
        }
        else if ( s_eqi ( word, "WRAPT" ) )
        {
          count = sscanf ( next, "%s%n", word, &width );
          next = next + width;
        }
        else
        {
        }
      }
//
//  TEXTURECOORDINATE2
//
      else if ( s_eqi ( level_name[level], "TEXTURECOORDINATE2" ) )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( s_eqi ( word, "POINT" ) )
        {
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "TEXTURECOORDINATE2: Bad data " << word << "\n";
        }
      }
//
//  TEXTURECOORDINATEBINDING
//
      else if ( s_eqi ( level_name[level], "TEXTURECOORDINATEBINDING" ) )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( s_eqi ( word, "VALUE" ) )
        {
          count = sscanf ( next, "%s%n", texture_binding, &width );
          next = next + width;
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "Bad data " << word << "\n";
        }
      }
//
//  TEXTURECOORDINDEX
//
      else if ( s_eqi ( level_name[level], "TEXTURECOORDINDEX" ) )
      {
        if ( strcmp ( word, "[" ) == 0 )
        {
          ivert = 0;
          iface_num = 0;
        }
        else if ( strcmp ( word, "]" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else
        {
          count = sscanf ( word, "%d%n", &jval, &width );

          if ( 0 < count )
          {
            if ( jval == - 1 )
            {
              ivert = 0;
            }
            else
            {
              if ( iface_num < FACE_MAX )
              {
                vertex_tex_uv[0][ivert][iface_num] = texture_temp[0][jval];
                vertex_tex_uv[1][ivert][iface_num] = texture_temp[1][jval];
              }

              ivert = ivert + 1;

              if ( ivert == face_order[iface_num] )
              {
                ivert = 0;
                iface_num = iface_num + 1;
              }
            }

          }
          else
          {
            bad_num = bad_num + 1;
            cout << "Bad data " << word << "\n";
          }

        }
      }
//
//  UKNOTVECTOR
//
      else if ( s_eqi ( level_name[level], "UKNOTVECTOR" ) )
      {
        if ( strcmp ( word, "[" ) == 0 )
        {
          continue;
        }
        else if ( strcmp ( word, "]" ) == 0 )
        {
          level = nlbrack - nrbrack;
          continue;
        }
        else
        {
          count = sscanf ( word, "%d%n", &jval, &width );
        }
      }
//
//  VECTOR
//
      else if ( s_eqi ( level_name[level], "VECTOR" ) )
      {
        if ( strcmp ( word, "[" ) == 0 )
        {
        }
        else if ( strcmp ( word, "]" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
//
//  (normal) VECTOR
//
        else if ( s_eqi ( level_name[level-1], "NORMAL" ) )
        {

          count = sscanf ( word, "%f%n", &rval, &width );

          if ( 0 < count )
          {
            if ( normal_num_temp < ORDER_MAX * FACE_MAX )
            {
              normal_temp[ixyz][normal_num_temp] = rval;
              ixyz = ixyz + 1;
              if ( ixyz == 3 )
              {
                ixyz = 0;
                normal_num_temp = normal_num_temp + 1;
              }
            }
          }
          else
          {
            bad_num = bad_num + 1;
            cout << "NORMAL VECTOR: bad data " << word << "\n";
          }
        }
      }
//
//  (vertexproperty) VERTEX
//
      else if ( s_eqi ( level_name[level], "VERTEX" ) )
      {
        if ( s_eqi ( level_name[level-1], "VERTEXPROPERTY" ) )
        {
          if ( strcmp ( word, "[" ) == 0 )
          {
            ixyz = 0;
            cor3_num_old = cor3_num;
          }
          else if ( strcmp ( word, "]" ) == 0 )
          {
            level = nlbrack - nrbrack;
          }
          else
          {
            count = sscanf ( word, "%f%n", &rval, &width );

            if ( 0 < count )
            {
              if ( cor3_num < COR3_MAX )
              {
                cor3[ixyz][cor3_num] = rval;
              }
              ixyz = ixyz + 1;
              if ( ixyz == 3 )
              {
                ixyz = 0;
                cor3_num = cor3_num + 1;
              }

            }
            else
            {
              bad_num = bad_num + 1;
              cout << "Bad data " << word << "\n";
            }
          }
        }
      }
//
//  (indexedtrianglestripset) VERTEXPROPERTY
//
      else if ( s_eqi ( level_name[level], "VERTEXPROPERTY" ) )
      {
        if ( strcmp ( word, "{" ) == 0 )
        {
        }
        else if ( strcmp ( word, "}" ) == 0 )
        {
          level = nlbrack - nrbrack;
        }
        else if ( s_eqi ( word, "VERTEX" ) )
        {
        }
        else if ( s_eqi ( word, "NORMAL" ) )
        {
          ixyz = 0;
        }
        else if ( s_eqi ( word, "MATERIALBINDING" ) )
        {
          count = sscanf ( next, "%s%n", word, &width );
          next = next + width;
        }
        else if ( s_eqi ( word, "NORMALBINDING" ) )
        {
          count = sscanf ( next, "%s%n", word, &width );
          next = next + width;
        }
        else
        {
          bad_num = bad_num + 1;
          cout << "Bad data " << word << "\n";
        }
      }
//
//  VKNOTVECTOR
//
      else if ( s_eqi ( level_name[level], "VKNOTVECTOR" ) )
      {
        if ( strcmp ( word, "[" ) == 0 )
        {
          continue;
        }
        else if ( strcmp ( word, "]" ) == 0 )
        {
          level = nlbrack - nrbrack;
          continue;
        }
        else
        {
          count = sscanf ( word, "%d%n", &jval, &width );
        }
      }
//
//  Any other word:
//
      else
      {
      }
    }
  }
//
//  Reset the transformation matrix to the identity,
//  because, presumably, we've applied it by now.
//
  tmat_init ( transform_matrix );

  return 0;
}
//****************************************************************************80

int iv_write ( FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    IV_WRITE writes graphics information to an Inventor file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    29 June 1999
//
//  Author:
//
//    John Burkardt
//
{
  int icor3;
  int iface;
  int itemp;
  int ivert;
  int j;
  int length;
  int text_num;

  text_num = 0;

  fprintf ( fileout, "#Inventor V2.0 ascii\n" );
  fprintf ( fileout, "\n" );
  fprintf ( fileout, "Separator {\n" );
  fprintf ( fileout, "  Info {\n" );
  fprintf ( fileout, "    string \"%s generated by IVCON.\"\n", fileout_name );
  fprintf ( fileout, "    string \"Original data in file %s.\"\n", filein_name );
  fprintf ( fileout, "  }\n" );
  fprintf ( fileout, "  Separator {\n" );
  text_num = text_num + 8;
//
//  LightModel:
//
//    BASE_COLOR ignores light sources, and uses only diffuse color
//      and transparency.  Even without normal vector information,
//      the object will show up.  However, you won't get shadow
//      and lighting effects.
//
//    PHONG uses the Phong lighting model, accounting for light sources
//      and surface orientation.  This is the default.  I believe
//      you need accurate normal vector information in order for this
//      option to produce nice pictures.
//
//    DEPTH ignores light sources, and calculates lighting based on
//      the location of the object within the near and far planes
//      of the current camera's view volume.
//
  fprintf ( fileout, "    LightModel {\n" );
  fprintf ( fileout, "      model PHONG\n" );
  fprintf ( fileout, "    }\n" );
  text_num = text_num + 3;
//
//  Transformation matrix.
//
  fprintf ( fileout, "    MatrixTransform { matrix\n" );
  fprintf ( fileout, "      %f %f %f %f\n", transform_matrix[0][0],
    transform_matrix[0][1], transform_matrix[0][2], transform_matrix[0][3] );
  fprintf ( fileout, "      %f %f %f %f\n", transform_matrix[1][0],
    transform_matrix[1][1], transform_matrix[1][2], transform_matrix[1][3] );
  fprintf ( fileout, "      %f %f %f %f\n", transform_matrix[2][0],
    transform_matrix[2][1], transform_matrix[2][2], transform_matrix[2][3] );
  fprintf ( fileout, "      %f %f %f %f\n", transform_matrix[3][0],
    transform_matrix[3][1], transform_matrix[3][2], transform_matrix[3][3] );
  fprintf ( fileout, "    }\n" );
  text_num = text_num + 6;
//
//  Material.
//
  fprintf ( fileout, "    Material {\n" );
  fprintf ( fileout, "      ambientColor  0.2 0.2 0.2\n" );
  fprintf ( fileout, "      diffuseColor  0.8 0.8 0.8\n" );
  fprintf ( fileout, "      emissiveColor 0.0 0.0 0.0\n" );
  fprintf ( fileout, "      specularColor 0.0 0.0 0.0\n" );
  fprintf ( fileout, "      shininess     0.2\n" );
  fprintf ( fileout, "      transparency  0.0\n" );
  fprintf ( fileout, "    }\n" );
  text_num = text_num + 8;
//
//  MaterialBinding
//
  fprintf ( fileout, "    MaterialBinding {\n" );
  fprintf ( fileout, "      value PER_VERTEX_INDEXED\n" );
  fprintf ( fileout, "    }\n" );
  text_num = text_num + 3;
//
//  NormalBinding
//
//    PER_VERTEX promises that we will write a list of normal vectors
//    in a particular order, namely, the normal vectors for the vertices
//    of the first face, then the second face, and so on.
//
//    PER_VERTEX_INDEXED promises that we will write a list of normal vectors,
//    and then, as part of the IndexedFaceSet, we will give a list of
//    indices referencing this normal vector list.
//
  fprintf ( fileout, "    NormalBinding {\n" );
  fprintf ( fileout, "      value PER_VERTEX_INDEXED\n" );
  fprintf ( fileout, "    }\n" );
  text_num = text_num + 3;
//
//  Texture2.
//
//  FLAW: We can only handle on texture right now.
//
  if ( 0 < texture_num )
  {
    fprintf ( fileout, "    Texture2 {\n" );
    fprintf ( fileout, "      filename \"%s\"\n", texture_name[0] );
    fprintf ( fileout, "      wrapS       REPEAT\n" );
    fprintf ( fileout, "      wrapT       REPEAT\n" );
    fprintf ( fileout, "      model       MODULATE\n" );
    fprintf ( fileout, "      blendColor  0.0 0.0 0.0\n" );
    fprintf ( fileout, "    }\n" );
    text_num = text_num + 7;
  }
//
//  TextureCoordinateBinding
//
  fprintf ( fileout, "    TextureCoordinateBinding {\n" );
  fprintf ( fileout, "      value PER_VERTEX_INDEXED\n" );
  fprintf ( fileout, "    }\n" );
  text_num = text_num + 3;
//
//  ShapeHints
//
  fprintf ( fileout, "    ShapeHints {\n" );
  fprintf ( fileout, "      vertexOrdering COUNTERCLOCKWISE\n" );
  fprintf ( fileout, "      shapeType UNKNOWN_SHAPE_TYPE\n" );
  fprintf ( fileout, "      faceType CONVEX\n" );
  fprintf ( fileout, "      creaseAngle 6.28319\n" );
  fprintf ( fileout, "    }\n" );
  text_num = text_num + 6;
//
//  Point coordinates.
//
  fprintf ( fileout, "    Coordinate3 {\n" );
  fprintf ( fileout, "      point [\n" );
  text_num = text_num + 2;

  for ( j = 0; j < cor3_num; j++ )
  {
    fprintf ( fileout, "        %f %f %f,\n", cor3[0][j], cor3[1][j],
      cor3[2][j] );
    text_num = text_num + 1;
  }
  fprintf ( fileout, "      ]\n" );
  fprintf ( fileout, "    }\n" );
  text_num = text_num + 2;
//
//  Texture coordinates.
//
  fprintf ( fileout, "    TextureCoordinate2 {\n" );
  fprintf ( fileout, "      point [\n" );
  text_num = text_num + 2;

  for ( iface = 0; iface < face_num; iface++ )
  {
    for ( ivert = 0; ivert < face_order[iface]; ivert++ )
    {
      fprintf ( fileout, "        %f %f,\n", vertex_tex_uv[0][ivert][iface],
        vertex_tex_uv[1][ivert][iface] );
      text_num = text_num + 1;
    }
  }
  fprintf ( fileout, "      ]\n" );
  fprintf ( fileout, "    }\n" );
  text_num = text_num + 2;
//
//  BaseColor.
//
  if ( 0 < color_num )
  {
    fprintf ( fileout, "    BaseColor {\n" );
    fprintf ( fileout, "      rgb [\n" );
    text_num = text_num + 2;

    for ( j = 0; j < color_num; j++ )
    {
      fprintf ( fileout, "        %f %f %f,\n", rgbcolor[0][j], rgbcolor[1][j],
        rgbcolor[2][j] );
      text_num = text_num + 1;
    }

    fprintf ( fileout, "      ]\n" );
    fprintf ( fileout, "    }\n" );
    text_num = text_num + 2;
  }
//
//  Normal vectors.
//    Use the normal vectors associated with nodes.
//
  if ( 0 < face_num )
  {
    fprintf ( fileout, "    Normal { \n" );
    fprintf ( fileout, "      vector [\n" );
    text_num = text_num + 2;

    for ( icor3 = 0; icor3 < cor3_num; icor3++ )
    {
      fprintf ( fileout, "        %f %f %f,\n",
        cor3_normal[0][icor3],
        cor3_normal[1][icor3],
        cor3_normal[2][icor3] );
      text_num = text_num + 1;
    }

    fprintf ( fileout, "      ]\n" );
    fprintf ( fileout, "    }\n" );
    text_num = text_num + 2;
  }
//
//  IndexedLineSet
//
  if ( 0 < line_num )
  {
    fprintf ( fileout, "    IndexedLineSet {\n" );
//
//  IndexedLineSet coordIndex
//
    fprintf ( fileout, "      coordIndex [\n" );
    text_num = text_num + 2;

    length = 0;

    for ( j = 0; j < line_num; j++ )
    {
      if ( length == 0 )
      {
        fprintf ( fileout, "       " );
      }

      fprintf ( fileout, " %d,", line_dex[j] );
      length = length + 1;

      if ( line_dex[j] == -1 || 10 <= length || j == line_num-1 )
      {
        fprintf ( fileout, "\n" );
        text_num = text_num + 1;
        length = 0;
      }
    }

    fprintf ( fileout, "      ]\n" );
    text_num = text_num + 1;
//
//  IndexedLineSet materialIndex.
//
    fprintf ( fileout, "      materialIndex [\n" );
    text_num = text_num + 1;

    length = 0;

    for ( j = 0; j < line_num; j++ )
    {
      if ( length == 0 )
      {
        fprintf ( fileout, "       " );
      }

      fprintf ( fileout, " %d,", line_material[j] );
      length = length + 1;

      if ( line_material[j] == -1 || 10 <= length || j == line_num-1 )
      {
        fprintf ( fileout, "\n" );
        text_num = text_num + 1;
        length = 0;
      }
    }

    fprintf ( fileout, "      ]\n" );
    fprintf ( fileout, "    }\n" );
    text_num = text_num + 2;
  }
//
//  IndexedFaceSet.
//
  if ( 0 < face_num )
  {
    fprintf ( fileout, "    IndexedFaceSet {\n" );
    fprintf ( fileout, "      coordIndex [\n" );
    text_num = text_num + 2;

    for ( iface = 0; iface < face_num; iface++ )
    {
      fprintf ( fileout, "       " );

      for ( ivert = 0; ivert < face_order[iface]; ivert++ )
      {
        fprintf ( fileout, " %d,", face[ivert][iface] );
      }
      fprintf ( fileout, " -1,\n" );
      text_num = text_num + 1;
    }

    fprintf ( fileout, "      ]\n" );
    text_num = text_num + 1;
//
//  IndexedFaceSet normalIndex
//
    fprintf ( fileout, "      normalIndex [\n" );
    text_num = text_num + 1;

    for ( iface = 0; iface < face_num; iface++ )
    {
      fprintf ( fileout, "       " );

      for ( ivert = 0; ivert < face_order[iface]; ivert++ )
      {
        fprintf ( fileout, " %d,", face[ivert][iface] );
      }
      fprintf ( fileout, " -1,\n" );
      text_num = text_num + 1;
    }
    fprintf ( fileout, "      ]\n" );
    text_num = text_num + 1;
//
//  IndexedFaceSet materialIndex
//
    fprintf ( fileout, "      materialIndex [\n" );
    text_num = text_num + 1;

    for ( iface = 0; iface < face_num; iface++ )
    {
      fprintf ( fileout, "       " );

      for ( ivert = 0; ivert < face_order[iface]; ivert++ )
      {
        fprintf ( fileout, " %d,", vertex_material[ivert][iface] );
      }
      fprintf ( fileout, " -1,\n" );
      text_num = text_num + 1;
    }

    fprintf ( fileout, "      ]\n" );
    text_num = text_num + 1;
//
//  IndexedFaceSet textureCoordIndex
//
    fprintf ( fileout, "      textureCoordIndex [\n" );
    text_num = text_num + 1;

    itemp = 0;

    for ( iface = 0; iface < face_num; iface++ )
    {
      fprintf ( fileout, "       " );

      for ( ivert = 0; ivert < face_order[iface]; ivert++ )
      {
        fprintf ( fileout, " %d,", itemp );
        itemp = itemp + 1;
      }
      fprintf ( fileout, " -1,\n" );
      text_num = text_num + 1;
    }

    fprintf ( fileout, "      ]\n" );

    fprintf ( fileout, "    }\n" );
    text_num = text_num + 2;
  }
//
//  Close up the Separator nodes.
//
  fprintf ( fileout, "  }\n" );
  fprintf ( fileout, "}\n" );
  text_num = text_num + 2;
//
//  Report.
//
  cout << "\n";
  cout << "IV_WRITE - Wrote " << text_num << " text lines;\n";

  return 0;
}
//****************************************************************************80

int i4vec_max ( int n, int *a )

//****************************************************************************80
//
//  Purpose:
//
//    I4VEC_MAX returns the maximum element in an I4VEC.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    09 October 1998
//
//  Author:
//
//    John Burkardt
//
{
  int  i;
  int *ia;
  int  imax;

  if ( n <= 0 )
  {
    imax = 0;
  }
  else
  {
    ia = a;
    imax = *ia;
    for ( i = 1; i < n; i++ )
    {
      ia = ia + 1;
      if ( imax < *ia )
      {
        imax = *ia;
      }
    }
  }
  return imax;
}

//****************************************************************************80

long int long_int_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    LONG_INT_READ reads a long int from a binary file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    24 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  union {
    long int yint;
    char ychar[4];
  } y;

  if ( byte_swap )
  {
    y.ychar[3] = fgetc ( filein );
    y.ychar[2] = fgetc ( filein );
    y.ychar[1] = fgetc ( filein );
    y.ychar[0] = fgetc ( filein );
  }
  else
  {
    y.ychar[0] = fgetc ( filein );
    y.ychar[1] = fgetc ( filein );
    y.ychar[2] = fgetc ( filein );
    y.ychar[3] = fgetc ( filein );
  }

  return y.yint;
}
//****************************************************************************80

int long_int_write ( FILE *fileout, long int int_val )

//****************************************************************************80
//
//  Purpose:
//
//    LONG_INT_WRITE writes a long int to a binary file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    14 October 1998
//
//  Author:
//
//    John Burkardt
//
{
  union {
    long int yint;
    char ychar[4];
  } y;

  y.yint = int_val;

  if ( byte_swap )
  {
    fputc ( y.ychar[3], fileout );
    fputc ( y.ychar[2], fileout );
    fputc ( y.ychar[1], fileout );
    fputc ( y.ychar[0], fileout );
  }
  else
  {
    fputc ( y.ychar[0], fileout );
    fputc ( y.ychar[1], fileout );
    fputc ( y.ychar[2], fileout );
    fputc ( y.ychar[3], fileout );
  }

  return 4;
}
//****************************************************************************80

void news ( )

//****************************************************************************80
//
//  Purpose:
//
//    NEWS reports the program change history.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    05 September 2003
//
//  Author:
//
//    John Burkardt
//
{
  cout << "\n";
  cout << "Recent changes:\n";
  cout << "\n";
  cout << "  05 September 2003\n";
  cout << "    Trying to sneak OFF_READ in.\n";
  cout << "    Replaced all C PRINTF calls by C++ COUT <<\n";
  cout << "  29 August 2003\n";
  cout << "    Added OFF_WRITE.\n";
  cout << "  04 July 2000\n";
  cout << "    Added preliminary XGL_WRITE.\n";
  cout << "  26 September 1999\n";
  cout << "    After ASE_READ, call NODE_TO_VERTEX_MAT\n";
  cout << "    and VERTEX_TO_FACE_MATERIAL.\n";
  cout << "  27 July 1999\n";
  cout << "    Corrected TMAT_ROT_VECTOR.\n";
  cout << "  17 July 1999\n";
  cout << "    Added null edge and face deletion.\n";
  cout << "    Corrected a string problem in SMF_READ.\n";
  cout << "  03 July 1999\n";
  cout << "    Fixed a problem with BINDING variables in SMF_READ.\n";
  cout << "  02 July 1999\n";
  cout << "    Added limited texture support in 3DS/IV.\n";
  cout << "  26 June 1999\n";
  cout << "    BYU_READ added.\n";
  cout << "  25 June 1999\n";
  cout << "    BYU_WRITE added.\n";
  cout << "  22 June 1999\n";
  cout << "    TRIB_READ added.\n";
  cout << "  16 June 1999\n";
  cout << "    TRIB_WRITE Greg Hood binary triangle output routine added.\n";
  cout << "  10 June 1999\n";
  cout << "    TRIA_WRITE Greg Hood ASCII triangle output routine added.\n";
  cout << "  09 June 1999\n";
  cout << "    TEC_WRITE TECPLOT output routine added.\n";
  cout << "    IV_READ and IV_WRITE use TRANSFORM_MATRIX now.\n";
  cout << "  26 May 1999\n";
  cout << "    LINE_PRUNE option added for VLA_WRITE.\n";
  cout << "  24 May 1999\n";
  cout << "    Added << command to append new graphics data to old.\n";
  cout << "    Stuck in first draft STLB_READ/STLB_WRITE routines.\n";
  cout << "    STLA_WRITE and STLB_WRITE automatically decompose \n";
  cout << "      non-triangular faces before writing.\n";
  cout << "  23 May 1999\n";
  cout << "    Stuck in first draft WRL_WRITE routine.\n";
  cout << "  22 May 1999\n";
  cout << "    Faces converted to lines before calling VLA_WRITE.\n";
  cout << "    Added UCD_WRITE.\n";
  cout << "    Added MATERIAL/PATCH/TAGGEDPOINTS fields in HRC_READ.\n";
  cout << "  17 May 1999\n";
  cout << "    Updated SMF_WRITE, SMF_READ to match code in IVREAD.\n";
  cout << "    Added transformation matrix routines.\n";
  cout << "  16 May 1999\n";
  cout << "    Zik Saleeba improved DXF support to handle polygons.\n";
  cout << "  15 April 1999\n";
  cout << "    Zik Saleeba added Golgotha GMOD file format support.\n";
  cout << "  03 December 1998\n";
  cout << "    Set up simple hooks in TDS_READ_MATERIAL_SECTION.\n";
  cout << "  02 December 1998\n";
  cout << "    Set up simple hooks for texture map names.\n";
  cout << "  19 November 1998\n";
  cout << "    IV_WRITE uses PER_VERTEX normal binding.\n";
  cout << "  18 November 1998\n";
  cout << "    Added node normals.\n";
  cout << "    Finally added the -RN option.\n";
  cout << "  17 November 1998\n";
  cout << "    Added face node ordering reversal option.\n";
  cout << "  20 October 1998\n";
  cout << "    Added DATA_REPORT.\n";
  cout << "  19 October 1998\n";
  cout << "    SMF_READ and SMF_WRITE added.\n";
  cout << "  16 October 1998\n";
  cout << "    Fixing a bug in IV_READ that chokes on ]} and other\n";
  cout << "    cases where brackets aren't properly spaced.\n";
  cout << "  11 October 1998\n";
  cout << "    Added face subset selection option S.\n";
  cout << "  09 October 1998\n";
  cout << "    Reworking normal vector treatments.\n";
  cout << "    Synchronizing IVREAD and IVCON.\n";
  cout << "    POV_WRITE added.\n";
  cout << "  02 October 1998\n";
  cout << "    IVCON reproduces BOX.3DS and CONE.3DS exactly.\n";
  cout << "  30 September 1998\n";
  cout << "    IVCON compiled on the PC.\n";
  cout << "    Interactive BYTE_SWAP option added for binary files.\n";
  cout << "  25 September 1998\n";
  cout << "    OBJECT_NAME made available to store object name.\n";
  cout << "  23 September 1998\n";
  cout << "    3DS binary files can be written.\n";
  cout << "  15 September 1998\n";
  cout << "    3DS binary files can be read.\n";
  cout << "  01 September 1998\n";
  cout << "    COR3_RANGE, FACE_NORMAL_AVE added.\n";
  cout << "    Major modifications to normal vectors.\n";
  cout << "  24 August 1998\n";
  cout << "    HRC_READ added.\n";
  cout << "  21 August 1998\n";
  cout << "    TXT_WRITE improved.\n";
  cout << "  20 August 1998\n";
  cout << "    HRC_WRITE can output lines as linear splines.\n";
  cout << "  19 August 1998\n";
  cout << "    Automatic normal computation for OBJ files.\n";
  cout << "    Added normal vector computation.\n";
  cout << "    HRC_WRITE is working.\n";
  cout << "  18 August 1998\n";
  cout << "    IV_READ/IV_WRITE handle BASECOLOR RGB properly now.\n";
  cout << "    Improved treatment of face materials and normals.\n";
  cout << "  17 August 1998\n";
  cout << "    ORDER_MAX increased to 35.\n";
  cout << "    FACE_PRINT routine added.\n";
  cout << "    INIT_DATA routine added.\n";
  cout << "  14 August 1998\n";
  cout << "    IV_READ is working.\n";
  cout << "  13 August 1998\n";
  cout << "    ASE_WRITE is working.\n";
  cout << "    IV_WRITE is working.\n";
  cout << "  12 August 1998\n";
  cout << "    ASE_READ is working.\n";
  cout << "  10 August 1998\n";
  cout << "    DXF_WRITE is working.\n";
  cout << "    DXF_READ is working.\n";
  cout << "  27 July 1998\n";
  cout << "    Interactive mode is working.\n";
  cout << "    OBJ_READ is working.\n";
  cout << "  25 July 1998\n";
  cout << "    OBJ_WRITE is working.\n";
  cout << "  24 July 1998\n";
  cout << "    DATA_CHECK checks the input data.\n";
  cout << "    VLA_READ is working.\n";
  cout << "    VLA_WRITE is working.\n";
  cout << "  23 July 1998\n";
  cout << "    STL_WRITE is working.\n";
  cout << "  22 July 1998\n";
  cout << "    STL_READ is working.\n";
  cout << "    TXT_WRITE is working.\n";
}
//****************************************************************************80

void node_to_vertex_material ( )

//****************************************************************************80
//
//  Purpose:
//
//    NODE_TO_VERTEX_MAT extends node material definitions to vertices.
//
//  Discussion:
//
//    A NODE is a point in space.
//    A VERTEX is a node as used in a particular face.
//    One node may be used as a vertex in several faces, or none.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    22 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  int iface;
  int ivert;
  int node;

  for ( iface = 0; iface < face_num; iface++ )
  {
    for ( ivert = 0; ivert < face_order[iface]; ivert++ )
    {
      node = face[ivert][iface];
      vertex_material[ivert][iface] = cor3_material[node];
    }
  }

  return;
}
//****************************************************************************80

int obj_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    OBJ_READ reads a Wavefront OBJ file.
//
//  Example:
//
//    #  magnolia.obj
//
//    mtllib ./vp.mtl
//
//    g
//    v -3.269770 -39.572201 0.876128
//    v -3.263720 -39.507999 2.160890
//    ...
//    v 0.000000 -9.988540 0.000000
//    g stem
//    s 1
//    usemtl brownskn
//    f 8 9 11 10
//    f 12 13 15 14
//    ...
//    f 788 806 774
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    20 October 1998
//
//  Author:
//
//    John Burkardt
//
{
  int count;
  int i;
  int ivert;
  char *next;
  char *next2;
  char *next3;
  int node;
  int vertex_normal_num;
  float r1;
  float r2;
  float r3;
  char token[LINE_MAX_LEN];
  char token2[LINE_MAX_LEN];
  int width;
//
//  Initialize.
//
  vertex_normal_num = 0;
//
//  Read the next line of the file into INPUT.
//
  while ( fgets ( input, LINE_MAX_LEN, filein ) != NULL )
  {
    text_num = text_num + 1;
//
//  Advance to the first nonspace character in INPUT.
//
    for ( next = input; *next != '\0' && ch_is_space ( *next ); next++ )
    {
    }
//
//  Skip blank lines and comments.
//
    if ( *next == '\0' )
    {
      continue;
    }

    if ( *next == '#' || *next == '$' )
    {
      comment_num = comment_num + 1;
      continue;
    }
//
//  Extract the first word in this line.
//
    sscanf ( next, "%s%n", token, &width );
//
//  Set NEXT to point to just after this token.
//
    next = next + width;
//
//  BEVEL
//  Bevel interpolation.
//
    if ( s_eqi ( token, "BEVEL" ) )
    {
      continue;
    }
//
//  BMAT
//  Basis matrix.
//
    else if ( s_eqi ( token, "BMAT" ) )
    {
      continue;
    }
//
//  C_INTERP
//  Color interpolation.
//
    else if ( s_eqi ( token, "C_INTERP" ) )
    {
      continue;
    }
//
//  CON
//  Connectivity between free form surfaces.
//
    else if ( s_eqi ( token, "CON" ) )
    {
      continue;
    }
//
//  CSTYPE
//  Curve or surface type.
//
    else if ( s_eqi ( token, "CSTYPE" ) )
    {
      continue;
    }
//
//  CTECH
//  Curve approximation technique.
//
    else if ( s_eqi ( token, "CTECH" ) )
    {
      continue;
    }
//
//  CURV
//  Curve.
//
    else if ( s_eqi ( token, "CURV" ) )
    {
      continue;
    }
//
//  CURV2
//  2D curve.
//
    else if ( s_eqi ( token, "CURV2" ) )
    {
      continue;
    }
//
//  D_INTERP
//  Dissolve interpolation.
//
    else if ( s_eqi ( token, "D_INTERP" ) )
    {
      continue;
    }
//
//  DEG
//  Degree.
//
    else if ( s_eqi ( token, "DEG" ) )
    {
      continue;
    }
//
//  END
//  End statement.
//
    else if ( s_eqi ( token, "END" ) )
    {
      continue;
    }
//
//  F V1 V2 V3
//    or
//  F V1/VT1/VN1 V2/VT2/VN2 ...
//    or
//  F V1//VN1 V2//VN2 ...
//
//  Face.
//  A face is defined by the vertices.
//  Optionally, slashes may be used to include the texture vertex
//  and vertex normal indices.
//
//  OBJ line node indices are 1 based rather than 0 based.
//  So we have to decrement them before loading them into FACE.
//
    else if ( s_eqi ( token, "F" ) )
    {
      ivert = 0;
      face_order[face_num] = 0;
//
//  Read each item in the F definition as a token, and then
//  take it apart.
//
      for ( ;; )
      {
        count = sscanf ( next, "%s%n", token2, &width );
        next = next + width;

        if ( count != 1 )
        {
          break;
        }

        count = sscanf ( token2, "%d%n", &node, &width );
        next2 = token2 + width;

        if ( count != 1 )
        {
          break;
        }

        if ( ivert < ORDER_MAX && face_num < FACE_MAX )
        {
          face[ivert][face_num] = node-1;
          vertex_material[ivert][face_num] = 0;
          face_order[face_num] = face_order[face_num] + 1;
        }
//
//  If there's a slash, skip to the next slash, and extract the
//  index of the normal vector.
//
        if ( *next2 == '/' )
        {
          for ( next3 = next2 + 1; next3 < token2 + LINE_MAX_LEN; next3++ )
          {
            if ( *next3 == '/' )
            {
              next3 = next3 + 1;
              count = sscanf ( next3, "%d%n", &node, &width );

              node = node - 1;
              if ( 0 <= node && node < vertex_normal_num )
              {
                for ( i = 0; i < 3; i++ )
                {
                  vertex_normal[i][ivert][face_num] = normal_temp[i][node];
                }
              }
              break;
            }
          }
        }
        ivert = ivert + 1;
      }
      face_num = face_num + 1;
    }
//
//  G
//  Group name.
//
    else if ( s_eqi ( token, "G" ) )
    {
      continue;
    }
//
//  HOLE
//  Inner trimming hole.
//
    else if ( s_eqi ( token, "HOLE" ) )
    {
      continue;
    }
//
//  L
//  I believe OBJ line node indices are 1 based rather than 0 based.
//  So we have to decrement them before loading them into LINE_DEX.
//
    else if ( s_eqi ( token, "L" ) )
    {
      for ( ;; )
      {
        count = sscanf ( next, "%d%n", &node, &width );
        next = next + width;

        if ( count != 1 )
        {
          break;
        }

        if ( line_num < LINES_MAX  )
        {
          line_dex[line_num] = node-1;
          line_material[line_num] = 0;
        }
        line_num = line_num + 1;

      }

      if ( line_num < LINES_MAX )
      {
        line_dex[line_num] = -1;
        line_material[line_num] = -1;
      }
      line_num = line_num + 1;

    }
//
//  LOD
//  Level of detail.
//
    else if ( s_eqi ( token, "LOD" ) )
    {
      continue;
    }
//
//  MG
//  Merging group.
//
    else if ( s_eqi ( token, "MG" ) )
    {
      continue;
    }
//
//  MTLLIB
//  Material library.
//

    else if ( s_eqi ( token, "MTLLIB" ) )
    {
      continue;
    }
//
//  O
//  Object name.
//
    else if ( s_eqi ( token, "O" ) )
    {
      continue;
    }
//
//  P
//  Point.
//
    else if ( s_eqi ( token, "P" ) )
    {
      continue;
    }
//
//  PARM
//  Parameter values.
//
    else if ( s_eqi ( token, "PARM" ) )
    {
      continue;
    }
//
//  S
//  Smoothing group
//
    else if ( s_eqi ( token, "S" ) )
    {
      continue;
    }
//
//  SCRV
//  Special curve.
//
    else if ( s_eqi ( token, "SCRV" ) )
    {
      continue;
    }
//
//  SHADOW_OBJ
//  Shadow casting.
//
    else if ( s_eqi ( token, "SHADOW_OBJ" ) )
    {
      continue;
    }
//
//  SP
//  Special point.
//
    else if ( s_eqi ( token, "SP" ) )
    {
      continue;
    }
//
//  STECH
//  Surface approximation technique.
//
    else if ( s_eqi ( token, "STECH" ) )
    {
      continue;
    }
//
//  STEP
//  Stepsize.
//
    else if ( s_eqi ( token, "CURV" ) )
    {
      continue;
    }
//
//  SURF
//  Surface.
//
    else if ( s_eqi ( token, "SURF" ) )
    {
      continue;
    }
//
//  TRACE_OBJ
//  Ray tracing.
//
    else if ( s_eqi ( token, "TRACE_OBJ" ) )
    {
      continue;
    }
//
//  TRIM
//  Outer trimming loop.
//
    else if ( s_eqi ( token, "TRIM" ) )
    {
      continue;
    }
//
//  USEMTL
//  Material name.
//
    else if ( s_eqi ( token, "USEMTL" ) )
    {
      continue;
    }
//
//  V X Y Z W
//  Geometric vertex.
//  W is optional, a weight for rational curves and surfaces.
//  The default for W is 1.
//
    else if ( s_eqi ( token, "V" ) )
    {
      sscanf ( next, "%e %e %e", &r1, &r2, &r3 );

      if ( cor3_num < COR3_MAX )
      {
        cor3[0][cor3_num] = r1;
        cor3[1][cor3_num] = r2;
        cor3[2][cor3_num] = r3;
      }

      cor3_num = cor3_num + 1;

    }
//
//  VN
//  Vertex normals.
//
    else if ( s_eqi ( token, "VN" ) )
    {
      sscanf ( next, "%e %e %e", &r1, &r2, &r3 );

      if ( vertex_normal_num < ORDER_MAX * FACE_MAX )
      {
        normal_temp[0][vertex_normal_num] = r1;
        normal_temp[1][vertex_normal_num] = r2;
        normal_temp[2][vertex_normal_num] = r3;
      }

      vertex_normal_num = vertex_normal_num + 1;

    }
//
//  VT
//  Vertex texture.
//
    else if ( s_eqi ( token, "VT" ) )
    {
      continue;
    }
//
//  VP
//  Parameter space vertices.
//
    else if ( s_eqi ( token, "VP" ) )
    {
      continue;
    }
//
//  Unrecognized
//
    else
    {
      bad_num = bad_num + 1;
    }

  }
  return 0;
}
//****************************************************************************80

int obj_write ( FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    OBJ_WRITE writes a Wavefront OBJ file.
//
//  Example:
//
//    #  magnolia.obj
//
//    mtllib ./vp.mtl
//
//    g
//    v -3.269770 -39.572201 0.876128
//    v -3.263720 -39.507999 2.160890
//    ...
//    v 0.000000 -9.988540 0.000000
//    g stem
//    s 1
//    usemtl brownskn
//    f 8 9 11 10
//    f 12 13 15 14
//    ...
//    f 788 806 774
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    01 September 1998
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, FILE *FILEOUT, a pointer to the output file.
//
{
  int i;
  int iface;
  int indexvn;
  int ivert;
  int k;
  bool next;
  int text_num;
  float w;
//
//  Initialize.
//
  text_num = 0;
  w = 1.0;

  fprintf ( fileout, "# %s created by IVCON.\n", fileout_name );
  fprintf ( fileout, "# Original data in %s.\n", filein_name );
  fprintf ( fileout, "\n" );
  fprintf ( fileout, "g %s\n", object_name );
  fprintf ( fileout, "\n" );

  text_num = text_num + 5;
//
//  V: vertex coordinates.
//
  for ( i = 0; i < cor3_num; i++ )
  {
    fprintf ( fileout, "v %f %f %f %f\n",
      cor3[0][i], cor3[1][i], cor3[2][i], w );
    text_num = text_num + 1;
  }
//
//  VN: Vertex face normal vectors.
//
  if ( 0 < face_num )
  {
    fprintf ( fileout, "\n" );
    text_num = text_num + 1;
  }

  for ( iface = 0; iface < face_num; iface++ )
  {
    for ( ivert = 0; ivert < face_order[iface]; ivert++ )
    {
      fprintf ( fileout, "vn %f %f %f\n", vertex_normal[0][ivert][iface],
        vertex_normal[1][ivert][iface], vertex_normal[2][ivert][iface] );
      text_num = text_num + 1;
    }
  }
//
//  F: faces.
//
  if ( 0 < face_num )
  {
    fprintf ( fileout, "\n" );
    text_num = text_num + 1;
  }

  indexvn = 0;

  for ( iface = 0; iface < face_num; iface++ )
  {
    fprintf ( fileout, "f" );
    for ( ivert = 0; ivert < face_order[iface]; ivert++ )
    {
      indexvn = indexvn + 1;
      fprintf ( fileout, " %d//%d", face[ivert][iface]+1, indexvn );
    }
    fprintf ( fileout, "\n" );
    text_num = text_num + 1;
  }
//
//  L: lines.
//
  if ( 0 < line_num )
  {
    fprintf ( fileout, "\n" );
    text_num = text_num + 1;
  }

  next = true;

  for ( i = 0; i < line_num; i++ )
  {
    k = line_dex[i];

    if ( k == -1 )
    {
      fprintf ( fileout, "\n" );
      text_num = text_num + 1;
      next = true;
    }
    else
    {
      if ( next )
      {
        fprintf ( fileout, "l" );
        next = false;
      }
      fprintf ( fileout, " %d", k+1 );
    }

  }

  fprintf ( fileout, "\n" );
  text_num = text_num + 1;
//
//  Report.
//
  cout << "\n";
  cout << "OBJ_WRITE - Wrote " << text_num << " text lines.\n";

  return 0;
}
//****************************************************************************80

int off_read ( ifstream &file_in )

//****************************************************************************80
//
//  Purpose:
//
//    OFF_READ reads a GEOMVIEW OFF file.
//
//  Example:
//
//    OFF
//    8  6  12
//    0.0 0.0 0.0
//    0.0 0.0 1.0
//    0.0 1.0 0.0
//    0.0 1.0 1.0
//    1.0 0.0 0.0
//    1.0 0.0 1.0
//    1.0 1.0 0.0
//    1.0 1.0 1.0
//    4  0 2 3 1
//    4  4 5 7 6
//    4  0 1 5 4
//    4  2 6 7 3
//    4  0 4 6 2
//    4  1 3 7 5
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    05 September 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, ifstream &FILE_IN, a pointer to the OFF file.
//
//    Output, int *COR3_NUM, the number of nodes.
//
//    Output, int *FACE_NUM, the number of faces.
{
  int cor3_num2;
  int edge_num;
  bool error;
  int face_num2;
  int ivec[21];
  int j;
  int last;
  char line[255];
  int noncomment_num;
  int order;
  float rvec[3];
  int text_num;
  int vert;
//
//  Read the optional "OFF" line, and the line with the number of nodes,
//  faces and edges.
//
  noncomment_num = 0;
  text_num = 0;

  while ( 1 )
  {
    file_in.getline ( line, sizeof ( line ) );

    if ( file_in.eof ( ) )
    {
      cout << "\n";
      cout << "OFF_READ - Fatal error!\n";
      cout << "  End-of-file before anything was read.\n";
      cout << "  Number of text lines read = " << text_num << "\n";
      return 1;
    }

    text_num = text_num + 1;

    if ( line[0] == '#' || s_len_trim ( line ) == 0 )
    {
      continue;
    }

    noncomment_num = noncomment_num + 1;

    if ( line[0] == 'O' &&
        line[1] == 'F' &&
        line[2] == 'F' )
    {
      if ( noncomment_num == 1 )
      {
        continue;
      }
      else
      {
        cout << "\n";
        cout << "OFF_READ - Fatal error!\n";
        cout << "  The 'OFF' tag, if used, must be the first noncomment\n";
        cout << "  line in the file.\n";
        cout << "  Number of text lines read = " << text_num << "\n";
        return 1;
      }
    }

    error = s_to_i4vec ( line, 3, ivec );

    if ( error )
    {
      cout << "\n";
      cout << "OFF_READ - Fatal error!\n";
      cout << "  Error from S_TO_I4VEC while trying to extract\n";
      cout << "  COR3_NUM FACE_NUM EDGE_NUM record.\n";
      cout << "  Number of text lines read = " << text_num << "\n";
      cout << "  Current line = \"" << line << "\"\n";
      return 1;
    }

    cor3_num = ivec[0];
    face_num = ivec[1];
    edge_num = ivec[2];
    break;

  }
//
//  Read the lines that define the vertex coordinates
//
//  Each line has the form "X Y Z"
//
  cor3_num2 = 0;

  while ( cor3_num2 < cor3_num )
  {
    file_in.getline ( line, sizeof ( line ) );

    if ( file_in.eof ( ) )
    {
      cout << "\n";
      cout << "OFF_READ - Fatal error!\n";
      cout << "  End of file while reading X Y Z coordinate data.\n";
      cout << "  Number of text lines read = " << text_num << "\n";
      return 1;
    }

    text_num = text_num + 1;

    if ( line[0] == '#' || s_len_trim ( line ) == 0 )
    {
      continue;
    }
    noncomment_num = noncomment_num + 1;

    error = s_to_r4vec ( line, 3, rvec );

    if ( error )
    {
      cout << "\n";
      cout << "OFF_READ - Fatal error!\n";
      cout << "  Error from S_TO_R4VEC while trying to extract X Y Z data.\n";
      cout << "  Number of text lines read = " << text_num << "\n";
      return 1;
    }

    for ( j = 0; j < 3; j++ )
    {
      cor3[j][cor3_num2] = rvec[j];
    }

    cor3_num2 = cor3_num2 + 1;
  }
//
//  Read the lines that define the faces.
//
//  Each line has the form " n  V1  V2  V3 ... Vn"
//
  face_num2 = 0;

  while ( face_num2 < face_num )
  {
    file_in.getline ( line, sizeof ( line ) );

    if ( file_in.eof ( ) )
    {
      cout << "\n";
      cout << "OFF_READ - Fatal error!\n";
      cout << "  End of file while reading face data.\n";
      cout << "  Number of text lines read = " << text_num << "\n";
      return 1;
    }

    text_num = text_num + 1;

    if ( line[0] == '#' || s_len_trim ( line ) == 0 )
    {
      continue;
    }

    noncomment_num = noncomment_num + 1;

    order = s_to_i4 ( line, &last, &error );

    if ( error )
    {
      cout << "\n";
      cout << "OFF_READ - Fatal error!\n";
      cout << "  Error from S_TO_I4 while extracting face order.\n";
      cout << "  Number of text lines read = " << text_num << "\n";
      return 1;
    }

    face_order[face_num2] = order;

    error = s_to_i4vec ( line, order+1, ivec );

    if ( error )
    {
      cout << "\n";
      cout << "OFF_READ - Fatal error!\n";
      cout << "  Error from S_TO_I4VEC while extracting face indices.\n";
      cout << "  Number of text lines read = " << text_num << "\n";
      return 1;
    }

    for ( vert = 0; vert < order; vert++ )
    {
      face[vert][face_num2] = ivec[vert+1];
    }

    face_num2 = face_num2 + 1;
  }

  cout << "\n";
  cout << "OFF_READ read " << text_num << " lines.\n";

  return 0;
}
//****************************************************************************80

int off_write ( FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    OFF_WRITE writes graphics information to a GEOMVIEW OFF file.
//
//  Example:
//
//    OFF
//    8  6  12
//    0.0 0.0 0.0
//    0.0 0.0 1.0
//    0.0 1.0 0.0
//    0.0 1.0 1.0
//    1.0 0.0 0.0
//    1.0 0.0 1.0
//    1.0 1.0 0.0
//    1.0 1.0 1.0
//    4  0 2 3 1
//    4  4 5 7 6
//    4  0 1 5 4
//    4  2 6 7 3
//    4  0 4 6 2
//    4  1 3 7 5
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    28 August 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, FILE *FILEOUT, a pointer to the output file.
//
{
  int edge_num;
  int i;
  int iface;
  int ivert;
  int j;
  int text_num;
//
//  "Magic Number"
//
  text_num = 0;
  fprintf ( fileout, "OFF\n" );
  text_num = text_num + 1;
//
//  Compute EDGE_NUM.
//
  edge_num = edge_count ( );
//
//  Counts.
//
  fprintf ( fileout, "%d  %d  %d\n", cor3_num, face_num, edge_num );
  text_num = text_num + 1;
//
//  Vertex coordinates.
//
  for ( j = 0; j < cor3_num; j++ )
  {
    for ( i = 0; i < 3; i++ )
    {
      fprintf ( fileout, "%f  ", cor3[i][j] );
    }
    fprintf ( fileout, "\n" );
    text_num = text_num + 1;
  }
//
//  Face count, and vertex indices.
//
  for ( iface = 0; iface < face_num; iface++ )
  {
    fprintf ( fileout, "%d  ", face_order[iface] );

    for ( ivert = 0; ivert < face_order[iface]; ivert++ )
    {
      fprintf ( fileout, "%d  ",  face[ivert][iface] );
    }
    fprintf ( fileout, "\n" );
    text_num = text_num + 1;

  }
//
//  Report.
//
  cout << "\n";
  cout << "OFF_WRITE - Wrote " << text_num
       << " text lines to " << fileout_name << "\n";

  return 0;
}
//****************************************************************************80

int pov_write ( FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    POV_WRITE writes graphics information to a POV file.
//
//  Example:
//
//    // cone.pov created by IVCON.
//    // Original data in cone.iv
//
//    #version 3.0
//    #include "colors.inc"
//    #include "shapes.inc"
//    global_settings { assumed_gamma 2.2 }
//
//    camera {
//     right < 4/3, 0, 0>
//     up < 0, 1, 0 >
//     sky < 0, 1, 0 >
//     angle 20
//     location < 0, 0, -300 >
//     look_at < 0, 0, 0>
//    }
//
//    light_source { < 20, 50, -100 > color White }
//
//    background { color SkyBlue }
//
//    #declare RedText = texture {
//      pigment { color rgb < 0.8, 0.2, 0.2> }
//      finish { ambient 0.2 diffuse 0.5 }
//    }
//
//    #declare BlueText = texture {
//      pigment { color rgb < 0.2, 0.2, 0.8> }
//      finish { ambient 0.2 diffuse 0.5 }
//    }
//    mesh {
//      smooth_triangle {
//        < 0.29, -0.29, 0.0>, < 0.0, 0.0, -1.0 >,
//        < 38.85, 10.03, 0.0>, < 0.0, 0.0, -1.0 >,
//        < 40.21, -0.29, 0.0>, <  0.0, 0.0, -1.0 >
//        texture { RedText } }
//        ...
//      smooth_triangle {
//        <  0.29, -0.29, 70.4142 >, < 0.0,  0.0, 1.0 >,
//        <  8.56,  -2.51, 70.4142 >, < 0.0,  0.0, 1.0 >,
//        <  8.85, -0.29, 70.4142 >, < 0.0,  0.0, 1.0 >
//        texture { BlueText } }
//    }
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    08 October 1998
//
//  Author:
//
//    John Burkardt
//
{
  int i;
  int j;
  int jj;
  int jlo;
  int k;
  int text_num;

  text_num = 0;
  fprintf ( fileout,  "// %s created by IVCON.\n", fileout_name );
  fprintf ( fileout,  "// Original data in %s.\n", filein_name );
  text_num = text_num + 2;
//
//  Initial declarations.
//
  fprintf ( fileout, "\n" );
  fprintf ( fileout, "#version 3.0\n" );
  fprintf ( fileout, "#include \"colors.inc\"\n" );
  fprintf ( fileout, "#include \"shapes.inc\"\n" );
  fprintf ( fileout, "global_settings { assumed_gamma 2.2 }\n" );
  fprintf ( fileout, "\n" );
  fprintf ( fileout, "camera {\n" );
  fprintf ( fileout, " right < 4/3, 0, 0>\n" );
  fprintf ( fileout, " up < 0, 1, 0 >\n" );
  fprintf ( fileout, " sky < 0, 1, 0 >\n" );
  fprintf ( fileout, " angle 20\n" );
  fprintf ( fileout, " location < 0, 0, -300 >\n" );
  fprintf ( fileout, " look_at < 0, 0, 0>\n" );
  fprintf ( fileout, "}\n" );
  fprintf ( fileout, "\n" );
  fprintf ( fileout, "light_source { < 20, 50, -100 > color White }\n" );
  fprintf ( fileout, "\n" );
  fprintf ( fileout, "background { color SkyBlue }\n" );

  text_num = text_num + 15;
//
//  Declare RGB textures.
//
  fprintf ( fileout, "\n" );
  fprintf ( fileout, "#declare RedText = texture {\n" );
  fprintf ( fileout, "  pigment { color rgb < 0.8, 0.2, 0.2> }\n" );
  fprintf ( fileout, "  finish { ambient 0.2 diffuse 0.5 }\n" );
  fprintf ( fileout, "}\n" );
  fprintf ( fileout, "\n" );
  fprintf ( fileout, "#declare GreenText = texture {\n" );
  fprintf ( fileout, "  pigment { color rgb < 0.2, 0.8, 0.2> }\n" );
  fprintf ( fileout, "  finish { ambient 0.2 diffuse 0.5 }\n" );
  fprintf ( fileout, "}\n" );
  fprintf ( fileout, "\n" );
  fprintf ( fileout, "#declare BlueText = texture {\n" );
  fprintf ( fileout, "  pigment { color rgb < 0.2, 0.2, 0.8> }\n" );
  fprintf ( fileout, "  finish { ambient 0.2 diffuse 0.5 }\n" );
  fprintf ( fileout, "}\n" );
//
//  Write one big object.
//
  fprintf ( fileout,  "mesh {\n" );
  text_num = text_num + 1;
//
//  Do the next face.
//
  for ( i = 0; i < face_num; i++ )
  {
//
//  Break the face up into triangles, anchored at node 1.
//
    for ( jlo = 0; jlo < face_order[i] - 2; jlo++ )
    {
      fprintf ( fileout, "  smooth_triangle {\n" );
      text_num = text_num + 1;

      for ( j = jlo; j < jlo + 3; j++ )
      {
        if ( j == jlo )
        {
          jj = 0;
        }
        else
        {
          jj = j;
        }

        k = face[jj][i];

        fprintf ( fileout, "<%f, %f, %f>, <%f, %f, %f>",
          cor3[0][k], cor3[1][k], cor3[2][k],
          vertex_normal[0][jj][i],
          vertex_normal[1][jj][i],
          vertex_normal[2][jj][i] );

        if ( j < jlo + 2 )
        {
          fprintf ( fileout, ",\n" );
        }
        else
        {
          fprintf ( fileout, "\n" );
        }
        text_num = text_num + 1;

      }

      if (i%6 == 1 )
      {
        fprintf ( fileout,  "texture { RedText } }\n" );
      }
      else if ( i%2 == 0 )
      {
        fprintf ( fileout,  "texture { BlueText } }\n" );
      }
      else
      {
        fprintf ( fileout,  "texture { GreenText } }\n" );
      }
      text_num = text_num + 1;

    }

  }

  fprintf ( fileout,  "}\n" );
  text_num = text_num + 1;
//
//  Report.
//
  cout << "\n";
  cout << "POV_WRITE - Wrote " << text_num << " text lines.\n";

  return 0;
}
//****************************************************************************80

int rcol_find ( float a[][COR3_MAX], int m, int n, float r[] )

//****************************************************************************80
//
//  Purpose:
//
//    RCOL_FIND finds if a vector occurs in a table.
//
//  Discussion:
//
//    Explicitly forcing the second dimension to be COR3_MAX is a kludge.
//    I have to figure out how to do this as pointer references.
//
//    Also, since the array is not sorted, this routine should not be carelessly
//    called repeatedly for really big values of N, because you'll waste a
//    lot of time.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    27 April 1999
//
//  Author:
//
//    John Burkardt
//
{
  int i;
  int icol;
  int j;

  icol = -1;

  for ( j = 0; j < n; j++ )
  {
    for ( i = 0; i < m; i++ )
    {
      if ( a[i][j] != r[i] )
      {
        break;
      }
      if ( i == m-1 )
      {
        return j;
      }
    }
  }

  return icol;
}
//****************************************************************************80

float rgb_to_hue ( float r, float g, float b )

//****************************************************************************80
//
//  Purpose:
//
//    RGB_TO_HUE converts (R,G,B) colors to a hue value between 0 and 1.
//
//  Discussion:
//
//    The hue computed here should be the same as the H value computed
//    for HLS and HSV, except that it ranges from 0 to 1 instead of
//    0 to 360.
//
//    A monochromatic color ( white, black, or a shade of gray) does not
//    have a hue.  This routine will return a special value of H = -1
//    for such cases.
//
//  Example:
//
//    Color    R    G    B     H
//
//    red  1.0  0.0  0.0   0.00
//    yellow   1.0  1.0  0.0   0.16
//    green    0.0  1.0  0.0   0.33
//    cyan     0.0  1.0  1.0   0.50
//    blue     0.0  0.0  1.0   0.67
//    magenta  1.0  0.0  1.0   0.83
//
//    black    0.0  0.0  0.0  -1.00
//    gray     0.5  0.5  0.5  -1.00
//    white    1.0  1.0  1.0  -1.00
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    22 May 1999
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, float R, G, B, the red, green and blue values of the color.
//    These values should be between 0 and 1.
//
//    Output, float RGB_TO_HUE, the corresponding hue of the color, or -1.0 if
//    the color is monochromatic.
//
{
  float h;
  float rgbmax;
  float rgbmin;
//
//  Make sure the colors are between 0 and 1.
//
  if ( r < 0.0 )
  {
    r = 0.0;
  }
  else if ( 1.0 < r )
  {
    r = 1.0;
  }

  if ( g < 0.0 )
  {
    g = 0.0;
  }
  else if ( 1.0 < g )
  {
    g = 1.0;
  }

  if ( b < 0.0 )
  {
    b = 0.0;
  }
  else if ( 1.0 < b )
  {
    b = 1.0;
  }
//
//  Compute the minimum and maximum of R, G and B.
//
  rgbmax = r;
  if ( rgbmax < g )
  {
    rgbmax = g;
  }
  if ( rgbmax < b )
  {
    rgbmax = b;
  }

  rgbmin = r;
  if ( g < rgbmin )
  {
    rgbmin = g;
  }
  if ( b < rgbmin )
  {
    rgbmin = b;
  }
//
//  If RGBMAX = RGBMIN, the color has no hue.
//
  if ( rgbmax == rgbmin )
  {
    h = - 1.0;
  }
//
//  Otherwise, we need to determine the dominant color.
//
  else
  {
    if ( r == rgbmax )
    {
      h = ( g - b ) / ( rgbmax - rgbmin );
    }
    else if ( g == rgbmax )
    {
      h = 2.0 + ( b - r ) / ( rgbmax - rgbmin );
    }
    else if ( b == rgbmax )
    {
      h = 4.0 + ( r - g ) / ( rgbmax - rgbmin );
    }

    h = h / 6.0;
//
//  Make sure H lies between 0 and 1.0.
//
    if ( h < 0.0 )
    {
      h = h + 1.0;
    }
    else if ( 1.0 < h )
    {
      h = h - 1.0;
    }

  }

  return h;
}
//****************************************************************************80

bool s_eqi ( char *s1, char *s2 )

//****************************************************************************80
//
//  Purpose:
//
//    S_EQI reports whether two strings are equal, ignoring case.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    05 May 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, char *S1, char *S2, pointers to two strings.
//
//    Output, bool S_EQI, is true if the strings are equal.
//
{
  int i;
  int nchar;
  int nchar1;
  int nchar2;

  nchar1 = strlen ( s1 );
  nchar2 = strlen ( s2 );
  nchar = i4_min ( nchar1, nchar2 );

//
//  The strings are not equal if they differ over their common length.
//
  for ( i = 0; i < nchar; i++ )
  {

    if ( ch_cap ( s1[i] ) != ch_cap ( s2[i] ) )
    {
      return false;
    }
  }
//
//  The strings are not equal if the longer one includes nonblanks
//  in the tail.
//
  if ( nchar < nchar1 )
  {
    for ( i = nchar; i < nchar1; i++ )
    {
      if ( s1[i] != ' ' )
      {
        return false;
      }
    }
  }
  else if ( nchar < nchar2 )
  {
    for ( i = nchar; i < nchar2; i++ )
    {
      if ( s2[i] != ' ' )
      {
        return false;
      }
    }
  }

  return true;

}
//****************************************************************************80

int s_len_trim ( char *s )

//****************************************************************************80
//
//  Purpose:
//
//    S_LEN_TRIM returns the length of a string to the last nonblank.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    26 April 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, char *S, a pointer to a string.
//
//    Output, int S_LEN_TRIM, the length of the string to the last nonblank.
//    If S_LEN_TRIM is 0, then the string is entirely blank.
//
{
  int n;
  char *t;

  n = strlen ( s );
  t = s + strlen ( s ) - 1;

  while ( 0 < n )
  {
    if ( *t != ' ' )
    {
      return n;
    }
    t--;
    n--;
  }

  return n;
}
//****************************************************************************80

int s_to_i4 ( char *s, int *last, bool *error )

//****************************************************************************80
//
//  Purpose:
//
//    S_TO_I4 reads an I4 from a string.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    13 June 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, char *S, a string to be examined.
//
//    Output, int *LAST, the last character of S used to make IVAL.
//
//    Output, bool *ERROR is TRUE if an error occurred.
//
//    Output, int *S_TO_I4, the integer value read from the string.
//    If the string is blank, then IVAL will be returned 0.
//
{
  char c;
  int i;
  int isgn;
  int istate;
  int ival;

  *error = false;
  istate = 0;
  isgn = 1;
  i = 0;
  ival = 0;

  while ( *s )
  {
    c = s[i];
    i = i + 1;
//
//  Haven't read anything.
//
    if ( istate == 0 )
    {
      if ( c == ' ' )
      {
      }
      else if ( c == '-' )
      {
        istate = 1;
        isgn = -1;
      }
      else if ( c == '+' )
      {
        istate = 1;
        isgn = + 1;
      }
      else if ( '0' <= c && c <= '9' )
      {
        istate = 2;
        ival = c - '0';
      }
      else
      {
        *error = true;
        return ival;
      }
    }
//
//  Have read the sign, expecting digits.
//
    else if ( istate == 1 )
    {
      if ( c == ' ' )
      {
      }
      else if ( '0' <= c && c <= '9' )
      {
        istate = 2;
        ival = c - '0';
      }
      else
      {
        *error = true;
        return ival;
      }
    }
//
//  Have read at least one digit, expecting more.
//
    else if ( istate == 2 )
    {
      if ( '0' <= c && c <= '9' )
      {
        ival = 10 * (ival) + c - '0';
      }
      else
      {
        ival = isgn * ival;
        *last = i - 1;
        return ival;
      }

    }
  }
//
//  If we read all the characters in the string, see if we're OK.
//
  if ( istate == 2 )
  {
    ival = isgn * ival;
    *last = s_len_trim ( s );
  }
  else
  {
    *error = true;
    *last = 0;
  }

  return ival;
}
//****************************************************************************80

bool s_to_i4vec ( char *s, int n, int ivec[] )

//****************************************************************************80
//
//  Purpose:
//
//    S_TO_I4VEC reads an I4VEC from a string.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 January 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, char *S, the string to be read.
//
//    Input, int N, the number of values expected.
//
//    Output, int IVEC[N], the values read from the string.
//
//    Output, bool S_TO_I4VEC, is TRUE if an error occurred.
//
{
  bool error;
  int i;
  int lchar;

  error = false;

  for ( i = 0; i < n; i++ )
  {
    ivec[i] = s_to_i4 ( s, &lchar, &error );

    if ( error )
    {
      cout << "\n";
      cout << "S_TO_I4VEC - Fatal error!\n";
      cout << "  S_TO_I4 returned error while reading item " << i << "\n";
      return error;
    }

    s = s + lchar;

  }

  return error;
}
//****************************************************************************80

float s_to_r4 ( char *s, int *lchar, bool *error )

//****************************************************************************80
//
//  Purpose:
//
//    S_TO_R4 reads an R4 from a string.
//
//  Discussion:
//
//    This routine will read as many characters as possible until it reaches
//    the end of the string, or encounters a character which cannot be
//    part of the real number.
//
//    Legal input is:
//
//       1 blanks,
//       2 '+' or '-' sign,
//       2.5 spaces
//       3 integer part,
//       4 decimal point,
//       5 fraction part,
//       6 'E' or 'e' or 'D' or 'd', exponent marker,
//       7 exponent sign,
//       8 exponent integer part,
//       9 exponent decimal point,
//      10 exponent fraction part,
//      11 blanks,
//      12 final comma or semicolon.
//
//    with most quantities optional.
//
//  Example:
//
//    S                 R
//
//    '1'               1.0
//    '     1   '       1.0
//    '1A'              1.0
//    '12,34,56'        12.0
//    '  34 7'          34.0
//    '-1E2ABCD'        -100.0
//    '-1X2ABCD'        -1.0
//    ' 2E-1'           0.2
//    '23.45'           23.45
//    '-4.2E+2'         -420.0
//    '17d2'            1700.0
//    '-14e-2'         -0.14
//    'e2'              100.0
//    '-12.73e-9.23'   -12.73 * 10.0**(-9.23)
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    07 August 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, char *S, the string containing the
//    data to be read.  Reading will begin at position 1 and
//    terminate at the end of the string, or when no more
//    characters can be read to form a legal real.  Blanks,
//    commas, or other nonnumeric data will, in particular,
//    cause the conversion to halt.
//
//    Output, int *LCHAR, the number of characters read from
//    the string to form the number, including any terminating
//    characters such as a trailing comma or blanks.
//
//    Output, bool *ERROR, is true if an error occurred.
//
//    Output, float S_TO_R4, the real value that was read from the string.
//
{
  char c;
  int ihave;
  int isgn;
  int iterm;
  int jbot;
  int jsgn;
  int jtop;
  int nchar;
  int ndig;
  float r;
  float rbot;
  float rexp;
  float rtop;
  char TAB = 9;

  nchar = s_len_trim ( s );
  *error = false;
  r = 0.0E+00;
  *lchar = -1;
  isgn = 1;
  rtop = 0.0E+00;
  rbot = 1.0E+00;
  jsgn = 1;
  jtop = 0;
  jbot = 1;
  ihave = 1;
  iterm = 0;

  for ( ; ; )
  {
    c = s[*lchar+1];
    *lchar = *lchar + 1;
//
//  Blank or TAB character.
//
    if ( c == ' ' || c == TAB )
    {
      if ( ihave == 2 )
      {
      }
      else if ( ihave == 6 || ihave == 7 )
      {
        iterm = 1;
      }
      else if ( 1 < ihave )
      {
        ihave = 11;
      }
    }
//
//  Comma.
//
    else if ( c == ',' || c == ';' )
    {
      if ( ihave != 1 )
      {
        iterm = 1;
        ihave = 12;
        *lchar = *lchar + 1;
      }
    }
//
//  Minus sign.
//
    else if ( c == '-' )
    {
      if ( ihave == 1 )
      {
        ihave = 2;
        isgn = -1;
      }
      else if ( ihave == 6 )
      {
        ihave = 7;
        jsgn = -1;
      }
      else
      {
        iterm = 1;
      }
    }
//
//  Plus sign.
//
    else if ( c == '+' )
    {
      if ( ihave == 1 )
      {
        ihave = 2;
      }
      else if ( ihave == 6 )
      {
        ihave = 7;
      }
      else
      {
        iterm = 1;
      }
    }
//
//  Decimal point.
//
    else if ( c == '.' )
    {
      if ( ihave < 4 )
      {
        ihave = 4;
      }
      else if ( 6 <= ihave && ihave <= 8 )
      {
        ihave = 9;
      }
      else
      {
        iterm = 1;
      }
    }
//
//  Exponent marker.
//
    else if ( ch_eqi ( c, 'E' ) || ch_eqi ( c, 'D' ) )
    {
      if ( ihave < 6 )
      {
        ihave = 6;
      }
      else
      {
        iterm = 1;
      }
    }
//
//  Digit.
//
    else if ( ihave < 11 && '0' <= c && c <= '9' )
    {
      if ( ihave <= 2 )
      {
        ihave = 3;
      }
      else if ( ihave == 4 )
      {
        ihave = 5;
      }
      else if ( ihave == 6 || ihave == 7 )
      {
        ihave = 8;
      }
      else if ( ihave == 9 )
      {
        ihave = 10;
      }

      ndig = ch_to_digit ( c );

      if ( ihave == 3 )
      {
        rtop = 10.0E+00 * rtop + ( float ) ndig;
      }
      else if ( ihave == 5 )
      {
        rtop = 10.0E+00 * rtop + ( float ) ndig;
        rbot = 10.0E+00 * rbot;
      }
      else if ( ihave == 8 )
      {
        jtop = 10 * jtop + ndig;
      }
      else if ( ihave == 10 )
      {
        jtop = 10 * jtop + ndig;
        jbot = 10 * jbot;
      }
    }
//
//  Anything else is regarded as a terminator.
//
    else
    {
      iterm = 1;
    }
//
//  If we haven't seen a terminator, and we haven't examined the
//  entire string, go get the next character.
//
    if ( iterm == 1 || nchar <= *lchar + 1 )
    {
      break;
    }

  }
//
//  If we haven't seen a terminator, and we have examined the
//  entire string, then we're done, and LCHAR is equal to NCHAR.
//
  if ( iterm != 1 && (*lchar) + 1 == nchar )
  {
    *lchar = nchar;
  }
//
//  Number seems to have terminated.  Have we got a legal number?
//  Not if we terminated in states 1, 2, 6 or 7!
//
  if ( ihave == 1 || ihave == 2 || ihave == 6 || ihave == 7 )
  {
    *error = true;
    return r;
  }
//
//  Number seems OK.  Form it.
//
  if ( jtop == 0 )
  {
    rexp = 1.0E+00;
  }
  else
  {
    if ( jbot == 1 )
    {
      rexp = pow ( ( double ) 10.0E+00, ( double ) ( jsgn * jtop ) );
    }
    else
    {
      rexp = jsgn * jtop;
      rexp = rexp / jbot;
      rexp = pow ( ( double ) 10.0E+00, ( double ) rexp );
    }

  }

  r = isgn * rexp * rtop / rbot;

  return r;
}
//****************************************************************************80

bool s_to_r4vec ( char *s, int n, float rvec[] )

//****************************************************************************80
//
//  Purpose:
//
//    S_TO_R4VEC reads an R4VEC from a string.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 February 2001
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, character ( len = * ) S, the string to be read.
//
//    Input, integer N, the number of values expected.
//
//    Output, real RVEC[N], the values read from the string.
//
//    Output, bool S_TO_R4VEC, is true if an error occurred.
//
{
  bool error;
  int i;
  int lchar;

  error = false;

  for ( i = 0; i < n; i++ )
  {
    rvec[i] = s_to_r4 ( s, &lchar, &error );

    if ( error )
    {
      return error;
    }

    s = s + lchar;

  }

  return error;
}
//****************************************************************************80

short int short_int_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    SHORT_INT_READ reads a short int from a binary file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    14 October 1998
//
//  Author:
//
//    John Burkardt
//
{
  unsigned char  c1;
  unsigned char  c2;
  short int      ival;

  c1 = fgetc ( filein );
  c2 = fgetc ( filein );

  ival = c1 | ( c2 << 8 );

  return ival;
}
//****************************************************************************80

int short_int_write ( FILE *fileout, short int short_int_val )

//****************************************************************************80
//
//  Purpose:
//
//    SHORT_INT_WRITE writes a short int to a binary file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    14 October 1998
//
//  Author:
//
//    John Burkardt
//
{
  union {
    short int yint;
    char ychar[2];
  } y;

  y.yint = short_int_val;

  if ( byte_swap )
  {
    fputc ( y.ychar[1], fileout );
    fputc ( y.ychar[0], fileout );
  }
  else
  {
    fputc ( y.ychar[0], fileout );
    fputc ( y.ychar[1], fileout );
  }

  return 2;
}
//****************************************************************************80

int smf_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    SMF_READ reads an SMF file.
//
//  Example:
//
//    #SMF2.0
//    #  cube_face.smf
//    #  This example demonstrates how an RGB color can be assigned to
//    #  each face of an object.
//    #
//    # First, define the geometry of the cube.
//    #
//    v 0.0  0.0  0.0
//    v 1.0  0.0  0.0
//    v 0.0  1.0  0.0
//    v 1.0  1.0  0.0
//    v 0.0  0.0  1.0
//    v 1.0  0.0  1.0
//    v 0.0  1.0  1.0
//    v 1.0  1.0  1.0
//    f 1 4 2
//    f 1 3 4
//    f 5 6 8
//    f 5 8 7
//    f 1 2 6
//    f 1 6 5
//    f 2 4 8
//    f 2 8 6
//    f 4 3 7
//    f 4 7 8
//    f 3 1 5
//    f 3 5 7
//    #
//    #  Colors will be bound 1 per face.
//    #
//    bind c face
//    c 1.0  0.0  0.0
//    c 1.0  0.0  0.0
//    c 0.0  1.0  0.0
//    c 0.0  1.0  0.0
//    c 0.0  0.0  1.0
//    c 0.0  0.0  1.0
//    c 1.0  1.0  0.0
//    c 1.0  1.0  0.0
//    c 0.0  1.0  1.0
//    c 0.0  1.0  1.0
//    c 1.0  0.0  1.0
//    c 1.0  0.0  1.0
//    #
//    #  Normal vectors will be bound 1 per face.
//    #
//    bind n face
//    n  0.0   0.0  -1.0
//    n  0.0   0.0  -1.0
//    n  0.0   0.0   1.0
//    n  0.0   0.0   1.0
//    n  0.0  -1.0   0.0
//    n  0.0  -1.0   0.0
//    n  1.0   0.0   0.0
//    n  1.0   0.0   0.0
//    n  0.0   1.0   0.0
//    n  0.0   1.0   0.0
//    n -1.0   0.0   0.0
//    n -1.0   0.0   0.0
//    #
//    #  Texture coordinate pairs will be bound 1 per face.
//    #
//    bind r face
//    r  0.0   0.0
//    r  0.0   0.1
//    r  0.0   0.2
//    r  0.0   0.3
//    r  0.1   0.0
//    r  0.1   0.1
//    r  0.1   0.2
//    r  0.1   0.3
//    r  0.2   0.0
//    r  0.2   0.1
//    r  0.2   0.2
//    r  0.2   0.3
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    03 July 1999
//
//  Author:
//
//    John Burkardt
//
{
  float angle;
  char axis;
  float b;
  char cnr[LINE_MAX_LEN];
  int count;
  float dx;
  float dy;
  int face_count;
  float g;
  int icor3_normal;
  int icor3_tex_uv;
  int iface_normal;
  int iface_tex_uv;
  int imat;
  int ivert;
  int level;
  char *next;
  int node;
  int node_count;
  float r;
  float r1;
  float r2;
  float r3;
  float rgba[4];
  char *string = "Dummy material";
  float sx;
  float sy;
  float sz;
  char token[LINE_MAX_LEN];
  char token2[LINE_MAX_LEN];
  char type[LINE_MAX_LEN];
  float u;
  float v;
  int vertex_base;
  int vertex_correction;
  int width;
  float x;
  float xvec[3];
  float y;
  float z;

  face_count = 0;
  icor3_normal = 0;
  icor3_tex_uv = 0;
  iface_normal = 0;
  iface_tex_uv = 0;
  level = 0;
  node_count = 0;
  vertex_base = 0;
  vertex_correction = 0;
//
//  Read the next line of the file into INPUT.
//
  while ( fgets ( input, LINE_MAX_LEN, filein ) != NULL )
  {
    text_num = text_num + 1;

    if ( debug )
    {
      cout << "SMF_READ: DEBUG: Reading line #" << text_num << "\n";
    }
//
//  Advance to the first nonspace character in INPUT.
//
    for ( next = input; *next != '\0' && ch_is_space ( *next ); next++ )
    {
    }
//
//  Skip blank lines.
//

    if ( *next == '\0' )
    {
      continue;
    }
//
//  Skip comment lines.
//
    if ( *next == '#' || *next == '$' )
    {
      comment_num = comment_num + 1;
      continue;
    }
//
//  Extract the first word in this line.
//
    sscanf ( next, "%s%n", token, &width );
//
//  Set NEXT to point to just after this token.
//
    next = next + width;
//
//  BEGIN
//  Reset the transformation matrix to identity.
//  Node numbering starts at zero again.  (Really, this is level based)
//  (Really should define a new transformation matrix, and concatenate.)
//  (Also, might need to keep track of level.)
//
    if ( s_eqi ( token, "BEGIN" ) )
    {
      level = level + 1;
      vertex_base = cor3_num;
      group_num = group_num + 1;
      tmat_init ( transform_matrix );
    }
//
//  BIND [c|n|r] [vertex|face]
//  Specify the binding for RGB color, Normal, or Texture.
//  Options are "vertex" or "face"
//
    else if ( s_eqi ( token, "BIND" ) )
    {
      sscanf ( next, "%s%n", cnr, &width );
      next = next + width;

      if ( debug )
      {
        cout << "CNR = " << cnr << "\n";
      }

      sscanf ( next, "%s%n", type, &width );
      next = next + width;

      if ( debug )
      {
        cout << "TYPE = " << type << "\n";
      }

      if ( s_eqi ( cnr, "C" ) )
      {
        if ( s_eqi ( type, "VERTEX" ) )
        {
          strcpy ( material_binding, "PER_VERTEX" );
        }
        else if ( s_eqi ( type, "FACE" ) )
        {
          strcpy ( material_binding, "PER_FACE" );
        }
      }
      else if ( s_eqi ( cnr, "N" ) )
      {
        if ( s_eqi ( type, "VERTEX" ) )
        {
          strcpy ( normal_binding, "PER_VERTEX" );
        }
        else if ( s_eqi ( type, "FACE" ) )
        {
          strcpy ( normal_binding, "PER_FACE" );
        }
      }
      else if ( s_eqi ( cnr, "R" ) )
      {
        if ( s_eqi ( type, "VERTEX" ) )
        {
          strcpy ( texture_binding, "PER_VERTEX" );
        }
        else if ( s_eqi ( type, "FACE" ) )
        {
          strcpy ( texture_binding, "PER_FACE" );
        }

      }

    }
//
//  C <r> <g> <b>
//  Specify an RGB color, with R, G, B between 0.0 and 1.0.
//
    else if ( s_eqi ( token, "C" ) )
    {
      sscanf ( next, "%f%n", &r, &width );
      next = next + width;

      sscanf ( next, "%f%n", &g, &width );
      next = next + width;

      sscanf ( next, "%f%n", &b, &width );
      next = next + width;
//
//    Set up a temporary material (R,G,B,1.0).
//    Add the material to the material database, or find the index of
//      a matching material already in.
//    Assign the material of the node or face to this index.
//
      rgba[0] = r;
      rgba[1] = g;
      rgba[2] = b;
      rgba[3] = 1.0;

      if ( material_num < MATERIAL_MAX )
      {
        for ( k = 0; k < 4; k++ )
        {
          material_rgba[k][material_num] = rgba[k];
        }

        imat = material_num;
        material_num = material_num + 1;

      }
      else
      {
        imat = 0;
      }

      if ( s_eqi ( material_binding, "PER_FACE" ) )
      {
        face_count = face_count + 1;
        face_material[face_count] = imat;
      }
      else if ( s_eqi ( material_binding, "PER_VERTEX" ) )
      {
        node_count = node_count + 1;
        cor3_material[node_count] = imat;

      }
      else
      {
        cout << "\n";
        cout << "SMF_READ - Fatal error!\n";
        cout << "  Material binding undefined!\n";
        return 1;

      }

    }
//
//  END
//  Drop down a level.
//
    else if ( s_eqi ( token, "END" ) )
    {
      level = level - 1;

      if ( level < 0 )
      {
        cout << "\n";
        cout << "SMF_READ - Fatal error!\n";
        cout << "  More END statements than BEGINs!\n";
        return 1;
      }
    }
//
//  F V1 V2 V3
//
//  Face.
//  A face is defined by the vertices.
//  Node indices are 1 based rather than 0 based.
//  So we have to decrement them before loading them into FACE.
//  Note that vertex indices start back at 0 each time a BEGIN is entered.
//  The strategy here won't handle nested BEGIN's, just one at a time.
//

    else if ( s_eqi ( token, "F" ) )
    {
      ivert = 0;
      face_order[face_num] = 0;
//
//  Read each item in the F definition as a token, and then
//  take it apart.
//
      for ( ;; )
      {
        count = sscanf ( next, "%s%n", token2, &width );
        next = next + width;

        if ( count != 1 )
        {
          break;
        }

        count = sscanf ( token2, "%d%n", &node, &width );

        if ( count != 1 )
        {
          break;
        }

        if ( ivert < ORDER_MAX && face_num < FACE_MAX )
        {
          face[ivert][face_num] = node - 1 + vertex_base;
          vertex_material[ivert][face_num] = 0;
          face_order[face_num] = face_order[face_num] + 1;
        }
        ivert = ivert + 1;
      }
      face_num = face_num + 1;
    }
//
//  N <x> <y> <z>
//  Specify a normal vector.
//
    else if ( s_eqi ( token, "N" ) )
    {
      sscanf ( next, "%f%n", &x, &width );
      next = next + width;

      sscanf ( next, "%f%n", &y, &width );
      next = next + width;

      sscanf ( next, "%f%n", &z, &width );
      next = next + width;

      if ( s_eqi ( normal_binding, "PER_FACE" ) )
      {
        face_normal[0][iface_normal] = x;
        face_normal[1][iface_normal] = y;
        face_normal[2][iface_normal] = z;

        iface_normal = iface_normal + 1;
      }
      else if ( s_eqi ( normal_binding, "PER_VERTEX" ) )
      {
        cor3_normal[0][icor3_normal] = x;
        cor3_normal[1][icor3_normal] = y;
        cor3_normal[2][icor3_normal] = z;

        icor3_normal = icor3_normal + 1;
      }
      else
      {
        cout << "\n";
        cout << "SMF_READ - Fatal error!\n";
        cout << "  Normal binding undefined!\n";
        return 1;

      }
    }
//
//  R <u> <v>
//  Specify a texture coordinate.
//
    else if ( s_eqi ( token, "R" ) )
    {
      sscanf ( next, "%f%n", &u, &width );
      next = next + width;

      sscanf ( next, "%f%n", &v, &width );
      next = next + width;

      if ( s_eqi ( texture_binding, "PER_FACE" ) )
      {
        face_tex_uv[0][iface_tex_uv] = u;
        face_tex_uv[1][iface_tex_uv] = v;

        icor3_tex_uv = icor3_tex_uv + 1;

      }
      else if ( s_eqi ( texture_binding, "PER_VERTEX" ) )
      {
        cor3_tex_uv[0][icor3_tex_uv] = u;
        cor3_tex_uv[1][icor3_tex_uv] = v;

        icor3_tex_uv = icor3_tex_uv + 1;
      }
      else
      {
        cout << "\n";
        cout << "SMF_READ - Fatal error!\n";
        cout << "  Texture binding undefined!\n";
        return 1;
      }

    }
//
//  ROT [x|y|z] <theta>
//
    else if ( s_eqi ( token, "ROT" ) )
    {
      sscanf ( next, "%c%n", &axis, &width );
      next = next + width;

      sscanf ( next, "%f%n", &angle, &width );
      next = next + width;

      tmat_rot_axis ( transform_matrix, transform_matrix, angle, axis );
    }
//
//  SCALE <sx> <sy> <sz>
//
    else if ( s_eqi ( token, "SCALE" ) )
    {
      sscanf ( next, "%f%n", &sx, &width );
      next = next + width;

      sscanf ( next, "%f%n", &sy, &width );
      next = next + width;

      sscanf ( next, "%f%n", &sz, &width );
      next = next + width;

      tmat_scale ( transform_matrix, transform_matrix, sx, sy, sz );
    }
//
//  SET VERTEX_CORRECTION <i>
//  Specify increment to add to vertex indices in file.
//
    else if ( s_eqi ( token, "SET" ) )
    {
      sscanf ( next, "%s%n", cnr, &width );
      next = next + width;

      sscanf ( next, "%d%n", &vertex_correction, &width );
      next = next + width;
    }
//
//  T_SCALE <dx> <dy>
//  Specify a scaling to texture coordinates.
//
    else if ( s_eqi ( token, "T_SCALE" ) )
    {
      sscanf ( next, "%f%n", &dx, &width );
      next = next + width;

      sscanf ( next, "%f%n", &dy, &width );
      next = next + width;
    }
//
//  T_TRANS <dx> <dy>
//  Specify a translation to texture coordinates.
//
    else if ( s_eqi ( token, "T_TRANS" ) )
    {
      sscanf ( next, "%f%n", &dx, &width );
      next = next + width;

      sscanf ( next, "%f%n", &dy, &width );
      next = next + width;
    }
//
//  TEX <filename>
//  Specify a filename containing the texture.
//  (ANY CHANCE THIS IS RIGHT?)
//  The compiler warns that STRING is not initialized.
//
    else if ( s_eqi ( token, "TEX" ) )
    {
      sscanf ( next, "%s%n", string, &width );

      for ( i = 0; i < LINE_MAX_LEN; i++ )
      {
        texture_name[texture_num][i] = string[i];
        if ( string[i] == '\0' )
        {
          break;
        }
      }

      texture_num = texture_num + 1;
    }
//
//  TRANS <dx> <dy> <dz>
//
    else if ( s_eqi ( token, "TRANS" ) )
    {
      sscanf ( next, "%f%n", &x, &width );
      next = next + width;

      sscanf ( next, "%f%n", &y, &width );
      next = next + width;

      sscanf ( next, "%f%n", &z, &width );
      next = next + width;

      tmat_trans ( transform_matrix, transform_matrix, x, y, z );
    }
//
//  V X Y Z
//  Geometric vertex.
//
    else if ( s_eqi ( token, "V" ) )
    {
      sscanf ( next, "%e %e %e", &r1, &r2, &r3 );

      xvec[0] = r1;
      xvec[1] = r2;
      xvec[2] = r3;
//
//  Apply current transformation matrix.
//  Right now, we can only handle one matrix, not a stack of
//  matrices representing nested BEGIN/END's.
//
      tmat_mxp ( transform_matrix, xvec, xvec );

      if ( cor3_num < COR3_MAX )
      {
        for ( i = 0; i < 3; i++ )
        {
          cor3[i][cor3_num] = xvec[i];
        }
      }

      cor3_num = cor3_num + 1;

    }
//
//  Unrecognized keyword.
//
    else
    {
      bad_num = bad_num + 1;

      if ( bad_num <= 10 )
      {
        cout << "\n";
        cout << "SMF_READ: Bad data on line " << text_num << ".\n";
      }
    }

  }
//
//  Extend the material definition
//  * from the face to the vertices and nodes, or
//  * from the vertices to the faces and nodes.
//
  if ( strcmp ( material_binding, "PER_FACE" ) == 0 )
  {
    face_to_vertex_material ( );

    vertex_to_node_material ( );

  }
  else if ( strcmp ( material_binding, "PER_VERTEX" ) == 0 )
  {
    node_to_vertex_material ( );

    vertex_to_face_material ( );

  }

  return 0;
}
//****************************************************************************80

int smf_write ( FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    SMF_WRITE writes graphics information to an SMF file.
//
//  Example:
//
//    #SMF2.0
//    #  cube_face.smf
//    #  This example demonstrates how an RGB color can be assigned to
//    #  each face of an object.
//    #
//    # First, define the geometry of the cube.
//    #
//    v 0.0  0.0  0.0
//    v 1.0  0.0  0.0
//    v 0.0  1.0  0.0
//    v 1.0  1.0  0.0
//    v 0.0  0.0  1.0
//    v 1.0  0.0  1.0
//    v 0.0  1.0  1.0
//    v 1.0  1.0  1.0
//    f 1 4 2
//    f 1 3 4
//    f 5 6 8
//    f 5 8 7
//    f 1 2 6
//    f 1 6 5
//    f 2 4 8
//    f 2 8 6
//    f 4 3 7
//    f 4 7 8
//    f 3 1 5
//    f 3 5 7
//    #
//    #  Colors will be bound 1 per face.
//    #
//    bind c face
//    c 1.0  0.0  0.0
//    c 1.0  0.0  0.0
//    c 0.0  1.0  0.0
//    c 0.0  1.0  0.0
//    c 0.0  0.0  1.0
//    c 0.0  0.0  1.0
//    c 1.0  1.0  0.0
//    c 1.0  1.0  0.0
//    c 0.0  1.0  1.0
//    c 0.0  1.0  1.0
//    c 1.0  0.0  1.0
//    c 1.0  0.0  1.0
//    #
//    #  Normal vectors will be bound 1 per face.
//    #
//    bind n face
//    n  0.0   0.0  -1.0
//    n  0.0   0.0  -1.0
//    n  0.0   0.0   1.0
//    n  0.0   0.0   1.0
//    n  0.0  -1.0   0.0
//    n  0.0  -1.0   0.0
//    n  1.0   0.0   0.0
//    n  1.0   0.0   0.0
//    n  0.0   1.0   0.0
//    n  0.0   1.0   0.0
//    n -1.0   0.0   0.0
//    n -1.0   0.0   0.0
//    #
//    #  Texture coordinate pairs will be bound 1 per face.
//    #
//    bind r face
//    r  0.0   0.0
//    r  0.0   0.1
//    r  0.0   0.2
//    r  0.0   0.3
//    r  0.1   0.0
//    r  0.1   0.1
//    r  0.1   0.2
//    r  0.1   0.3
//    r  0.2   0.0
//    r  0.2   0.1
//    r  0.2   0.2
//    r  0.2   0.3
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    05 July 1999
//
//  Author:
//
//    John Burkardt
//
{
  int i;
  int icor3;
  int iface;
  int imat;
  int ivert;
  int text_num;
//
//  Initialize.
//
  text_num = 0;

  fprintf ( fileout, "#$SMF 2.0\n" );
  fprintf ( fileout, "#$vertices %d\n", cor3_num );
  fprintf ( fileout, "#$faces %d\n", face_num );
  fprintf ( fileout, "#\n" );
  fprintf ( fileout, "# %s created by IVCON.\n", fileout_name );
  fprintf ( fileout, "# Original data in %s.\n", filein_name );
  fprintf ( fileout, "#\n" );

  text_num = text_num + 7;
//
//  V: vertex coordinates.
//
  for ( i = 0; i < cor3_num; i++ )
  {
    fprintf ( fileout, "v %f %f %f\n",
      cor3[0][i], cor3[1][i], cor3[2][i] );
    text_num = text_num + 1;
  }
//
//  F: faces.
//
  if ( 0 < face_num )
  {
    fprintf ( fileout, "\n" );
    text_num = text_num + 1;
  }

  for ( iface = 0; iface < face_num; iface++ )
  {
    fprintf ( fileout, "f" );
    for ( ivert = 0; ivert < face_order[iface]; ivert++ )
    {
      fprintf ( fileout, " %d", face[ivert][iface]+1 );
    }
    fprintf ( fileout, "\n" );
    text_num = text_num + 1;
  }
//
//  Material binding.
//
  fprintf ( fileout, "bind c vertex\n" );
  text_num = text_num + 1;
//
//  Material RGB values at each node.
//
  for ( icor3 = 0; icor3 < cor3_num; icor3++ )
  {
    imat = cor3_material[icor3];

    fprintf ( fileout, "c %f %f %f\n", material_rgba[0][imat],
      material_rgba[1][imat], material_rgba[2][imat] );

    text_num = text_num + 1;
  }
//
//  Normal binding.
//
  fprintf ( fileout, "bind n vertex\n" );
  text_num = text_num + 1;
//
//  Normal vector at each node.
//
  for ( icor3 = 0; icor3 < cor3_num; icor3++ )
  {
    fprintf ( fileout, "n %f %f %f\n", cor3_normal[0][icor3],
      cor3_normal[1][icor3], cor3_normal[2][icor3] );

    text_num = text_num + 1;
  }

  if ( 0 < texture_num )
  {
//
//  Texture filename.
//
    fprintf ( fileout, "tex %s\n", texture_name[0] );
    text_num = text_num + 1;
//
//  Texture binding.
//
    fprintf ( fileout, "bind r vertex\n" );
    text_num = text_num + 1;
//
//  Texture coordinates at each node.
//
    for ( icor3 = 0; icor3 < cor3_num; icor3++ )
    {
      fprintf ( fileout, "r %f %f\n", cor3_tex_uv[0][icor3],
        cor3_tex_uv[1][icor3] );
      text_num = text_num + 1;
    }

  }
//
//  Report.
//
  cout << "\n";
  cout << "SMF_WRITE - Wrote " << text_num << " text lines.\n";

  return 0;
}
//****************************************************************************80

void sort_heap_external ( int n, int *indx, int *i, int *j, int isgn )

//****************************************************************************80
//
//  Purpose:
//
//    SORT_HEAP_EXTERNAL externally sorts a list of items into linear order.
//
//  Discussion:
//
//    The actual list is not passed to the routine.  Hence it may
//    consist of ints, reals, numbers, names, etc.  The user,
//    after each return from the routine, will be asked to compare or
//    interchange two items.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    20 May 1999
//
//  Author:
//
//    Original FORTRAN77 version by Albert Nijenhuis, Herbert Wilf.
//    C++ version by John Burkardt.
//
//  Parameters:
//
//    Input, int N, the length of the input list.
//
//    Input/output, int *INDX.
//
//    The user must set INDX to 0 before the first call.
//
//    On return,
//
//      if INDX is greater than 0, the user must interchange
//      items I and J and recall the routine.
//
//      If INDX is less than 0, the user is to compare items I
//      and J and return in ISGN a negative value if I is to
//      precede J, and a positive value otherwise.
//
//      If INDX is 0, the sorting is done.
//
//    Output, int *I, *J.  On return with INDX positive,
//    elements I and J of the user's list should be
//    interchanged.  On return with INDX negative, elements I
//    and J are to be compared by the user.
//
//    Input, int ISGN. On return with INDX negative, the
//    user should compare elements I and J of the list.  If
//    item I is to precede item J, set ISGN negative,
//    otherwise set ISGN positive.
//
{
  static int k = 0;
  static int k1 = 0;
  static int n1 = 0;

  if ( *indx == 0 )
  {

    n1 = n;
    k = n / 2;
    k1 = k;

  }
  else if ( *indx < 0 )
  {

    if ( *indx == -2 )
    {

      if ( isgn < 0 )
      {
        *i = *i + 1;
      }

      *j = k1 - 1;
      k1 = *i + 1;
      *indx = - 1;
      return;

    }

    if ( 0 < isgn )
    {
      *indx = 2;
      return;
    }

    if ( k <= 1 )
    {

      if ( n1 == 1 )
      {
        *indx = 0;
      }
      else
      {
        *i = n1 - 1;
        n1 = n1 - 1;
        *j = 0;
        *indx = 1;
      }

      return;

    }

    k = k - 1;
    k1 = k;

  }
  else if ( *indx == 1 )
  {
    k1 = k;
  }

  for ( ;; )
  {

    *i = 2 * k1 - 1;

    if ( *i == n1 - 1 )
    {
      *j = k1 - 1;
      k1 = *i + 1;
      *indx = - 1;
      return;
    }
    else if ( *i <= n1 - 1 )
    {
      *j = *i + 1;
      *indx = - 2;
      return;
    }

    if ( k <= 1 )
    {
      break;
    }

    k = k - 1;
    k1 = k;
  }

  if ( n1 == 1 )
  {
    *indx = 0;
  }
  else
  {
    *i = n1 - 1;
    n1 = n1 - 1;
    *j = 0;
    *indx = 1;
  }

  return;
}
//****************************************************************************80

int stla_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    STLA_READ reads an ASCII STL (stereolithography) file.
//
//  Example:
//
//    solid MYSOLID
//      facet normal 0.4 0.4 0.2
//        outerloop
//          vertex  1.0 2.1 3.2
//          vertex  2.1 3.7 4.5
//          vertex  3.1 4.5 6.7
//        endloop
//      endfacet
//      ...
//      facet normal 0.2 0.2 0.4
//        outerloop
//          vertex  2.0 2.3 3.4
//          vertex  3.1 3.2 6.5
//          vertex  4.1 5.5 9.0
//        endloop
//      endfacet
//    endsolid MYSOLID
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    20 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    3D Systems, Inc,
//    Stereolithography Interface Specification,
//    October 1989.
//
{
  int   count;
  int   i;
  int   icor3;
  int   ivert;
  char *next;
  float r1;
  float r2;
  float r3;
  float r4;
  float temp[3];
  char  token[LINE_MAX_LEN];
  int   width;
//
//  Read the next line of the file into INPUT.
//
  while ( fgets ( input, LINE_MAX_LEN, filein ) != NULL )
  {
    text_num = text_num + 1;
//
//  Advance to the first nonspace character in INPUT.
//
    for ( next = input; *next != '\0' && ch_is_space ( *next ); next++ )
    {
    }
//
//  Skip blank lines and comments.
//
    if ( *next == '\0' || *next == '#' || *next == '!' || *next == '$' )
    {
      continue;
    }
//
//  Extract the first word in this line.
//
    sscanf ( next, "%s%n", token, &width );
//
//  Set NEXT to point to just after this token.
//
    next = next + width;
//
//  FACET
//
    if ( s_eqi ( token, "facet" ) )
    {
//
//  Get the XYZ coordinates of the normal vector to the face.
//
      sscanf ( next, "%*s %e %e %e", &r1, &r2, &r3 );

      if ( face_num < FACE_MAX )
      {
        face_normal[0][face_num] = r1;
        face_normal[1][face_num] = r2;
        face_normal[2][face_num] = r3;
      }

      fgets ( input, LINE_MAX_LEN, filein );
      text_num = text_num + 1;

      ivert = 0;

      for ( ;; )
      {
        fgets ( input, LINE_MAX_LEN, filein );
        text_num = text_num + 1;

        count = sscanf ( input, "%*s %e %e %e", &r1, &r2, &r3 );

        if ( count != 3 )
        {
          break;
        }

        temp[0] = r1;
        temp[1] = r2;
        temp[2] = r3;

        if ( cor3_num < 1000 )
        {
          icor3 = rcol_find ( cor3, 3, cor3_num, temp );
        }
        else
        {
          icor3 = -1;
        }

        if ( icor3 == -1 )
        {
          icor3 = cor3_num;

          if ( cor3_num < COR3_MAX )
          {
            for ( i = 0; i < 3; i++ )
            {
              cor3[i][cor3_num] = temp[i];
            }
          }
          cor3_num = cor3_num + 1;
        }
        else
        {
          dup_num = dup_num + 1;
        }

        if ( ivert < ORDER_MAX && face_num < FACE_MAX )
        {
          face[ivert][face_num] = icor3;
          vertex_material[ivert][face_num] = 0;
          for ( i = 0; i < 3; i++ )
          {
            vertex_normal[i][ivert][face_num] = face_normal[i][face_num];
          }
        }

        ivert = ivert + 1;
      }

      fgets ( input, LINE_MAX_LEN, filein );
      text_num = text_num + 1;

      if ( face_num < FACE_MAX )
      {
        face_order[face_num] = ivert;
      }

      face_num = face_num + 1;

    }
//
//  COLOR
//

    else if ( s_eqi ( token, "color" ) )
    {
      sscanf ( next, "%*s %f %f %f %f", &r1, &r2, &r3, &r4 );
    }
//
// SOLID
//
    else if ( s_eqi ( token, "solid" ) )
    {
      object_num = object_num + 1;
    }
//
// ENDSOLID
//
    else if ( s_eqi ( token, "endsolid" ) )
    {
    }
//
//  Unexpected or unrecognized.
//
    else
    {
      cout << "\n";
      cout << "STLA_READ - Fatal error!\n";
      cout << "  Unrecognized first word on line.\n";
      return 1;
    }

  }
  return 0;
}
//****************************************************************************80

int stla_write ( FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    STLA_WRITE writes an ASCII STL (stereolithography) file.
//
//  Discussion:
//
//    The polygons in an STL file should only be triangular.  This routine
//    will try to automatically decompose higher-order polygonal faces into
//    suitable triangles, without actually modifying the internal graphics
//    data.
//
//  Example:
//
//    solid MYSOLID
//      facet normal 0.4 0.4 0.2
//        outerloop
//          vertex  1.0 2.1 3.2
//          vertex  2.1 3.7 4.5
//          vertex  3.1 4.5 6.7
//        endloop
//      endfacet
//      ...
//      facet normal 0.2 0.2 0.4
//        outerloop
//          vertex  2.0 2.3 3.4
//          vertex  3.1 3.2 6.5
//          vertex  4.1 5.5 9.0
//        endloop
//      endfacet
//    endsolid
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    01 September 1998
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    3D Systems, Inc,
//    Stereolithography Interface Specification,
//    October 1989.
//
{
  int icor3;
  int iface;
  int jvert;
  int face_num2;
  int text_num;
//
//  Initialize.
//
  text_num = 0;
  face_num2 = 0;

  fprintf ( fileout, "solid MYSOLID created by IVCON, original data in %s\n",
    filein_name );

  text_num = text_num + 1;

  for ( iface = 0; iface < face_num; iface++ )
  {
    for ( jvert = 2; jvert < face_order[iface]; jvert++ )
    {
      face_num2 = face_num2 + 1;

      fprintf ( fileout, "  facet normal %f %f %f\n",
        face_normal[0][iface], face_normal[1][iface], face_normal[2][iface] );

      fprintf ( fileout, "    outer loop\n" );

      icor3 = face[0][iface];
      fprintf ( fileout, "      vertex %f %f %f\n",
        cor3[0][icor3], cor3[1][icor3], cor3[2][icor3] );

      icor3 = face[jvert-1][iface];
      fprintf ( fileout, "      vertex %f %f %f\n",
        cor3[0][icor3], cor3[1][icor3], cor3[2][icor3] );

      icor3 = face[jvert][iface];
      fprintf ( fileout, "      vertex %f %f %f\n",
        cor3[0][icor3], cor3[1][icor3], cor3[2][icor3] );

      fprintf ( fileout, "    endloop\n" );
      fprintf ( fileout, "  endfacet\n" );
      text_num = text_num + 7;
    }
  }

  fprintf ( fileout, "endsolid MYSOLID\n" );
  text_num = text_num + 1;
//
//  Report.
//
  cout << "\n";
  cout << "STLA_WRITE - Wrote " << text_num << " text lines.\n";

  if ( face_num != face_num2 )
  {
    cout << "  Number of faces in original data was " << face_num << ".\n";
    cout << "  Number of triangular faces in decomposed data is "
         << face_num2 << ".\n";
  }

  return 0;
}
//****************************************************************************80

int stlb_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    STLB_READ reads a binary STL (stereolithography) file.
//
//  Example:
//
//    80 byte string = header containing nothing in particular
//
//    4 byte int = number of faces
//
//    For each face:
//
//      3 4-byte floats = components of normal vector to face;
//      3 4-byte floats = coordinates of first node;
//      3 4-byte floats = coordinates of second node;
//      3 4-byte floats = coordinates of third and final node;
//        2-byte int = attribute, whose value is 0.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    24 May 1999
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    3D Systems, Inc,
//    Stereolithography Interface Specification,
//    October 1989.
//
{
  short int attribute = 0;
  char c;
  float cvec[3];
  int icor3;
  int i;
  int iface;
  int ivert;
//
//  80 byte Header.
//
  for ( i = 0; i < 80; i++ )
  {
    c = ch_read ( filein );
    if ( debug )
    {
      cout << c << "\n";
    }
    bytes_num = bytes_num + 1;
 }
//
//  Number of faces.
//
  face_num = long_int_read ( filein );
  bytes_num = bytes_num + 4;
//
//  For each (triangular) face,
//    components of normal vector,
//    coordinates of three vertices,
//    2 byte "attribute".
//
  for ( iface = 0; iface < face_num; iface++ )
  {
    face_order[iface] = 3;
    face_material[iface] = 0;

    for ( i = 0; i < 3; i++ )
    {
      face_normal[i][iface] = float_read ( filein );
      bytes_num = bytes_num + 4;
    }

    for ( ivert = 0; ivert < face_order[iface]; ivert++ )
    {
      for ( i = 0; i < 3; i++ )
      {
        cvec[i] = float_read ( filein );
        bytes_num = bytes_num + 4;
      }

      if ( cor3_num < 1000 )
      {
        icor3 = rcol_find ( cor3, 3, cor3_num, cvec );
      }
      else
      {
        icor3 = -1;
      }

      if ( icor3 == -1 )
      {
        icor3 = cor3_num;
        if ( cor3_num < COR3_MAX )
        {
          cor3[0][cor3_num] = cvec[0];
          cor3[1][cor3_num] = cvec[1];
          cor3[2][cor3_num] = cvec[2];
        }
        cor3_num = cor3_num + 1;
      }
      else
      {
        dup_num = dup_num + 1;
      }

      face[ivert][iface] = icor3;

    }
    attribute = short_int_read ( filein );
    if ( debug )
    {
      cout << "ATTRIBUTE = " << attribute << "\n";
    }
    bytes_num = bytes_num + 2;
  }

  return 0;
}
//****************************************************************************80

int stlb_write ( FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    STLB_WRITE writes a binary STL (stereolithography) file.
//
//  Discussion:
//
//    The polygons in an STL file should only be triangular.  This routine
//    will try to automatically decompose higher-order polygonal faces into
//    suitable triangles, without actually modifying the internal graphics
//    data.
//
//  Example:
//
//    80 byte string = header containing nothing in particular
//
//    4 byte int = number of faces
//
//    For each face:
//
//      3 4-byte floats = components of normal vector to face;
//      3 4-byte floats = coordinates of first node;
//      3 4-byte floats = coordinates of second node;
//      3 4-byte floats = coordinates of third and final node;
//        2-byte int = attribute, whose value is 0.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    24 May 1999
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    3D Systems, Inc,
//    Stereolithography Interface Specification,
//    October 1989.
//
{
  short int attribute = 0;
  char c;
  int i;
  int icor3;
  int iface;
  int jvert;
  int face_num2;
//
//  Write the 80 byte header.
//
  for ( i = 0; i < 80; i++ )
  {
    c = ' ';
    bytes_num = bytes_num + ch_write ( fileout, c );
  }
//
//  Number of faces.
//
  face_num2 = 0;
  for ( iface = 0; iface < face_num; iface++ )
  {
    face_num2 = face_num2 + face_order[iface] - 2;
  }

  bytes_num = bytes_num + long_int_write ( fileout, face_num2 );
//
//  For each (triangular) face,
//    components of normal vector,
//    coordinates of three vertices,
//    2 byte "attribute".
//
  for ( iface = 0; iface < face_num; iface++ )
  {
    for ( jvert = 2; jvert < face_order[iface]; jvert++ )
    {
      for ( i = 0; i < 3; i++ )
      {
        bytes_num = bytes_num + float_write ( fileout, face_normal[i][iface] );
      }

      icor3 = face[0][iface];
      for ( i = 0; i < 3; i++ )
      {
        bytes_num = bytes_num + float_write ( fileout, cor3[i][icor3] );
      }

      icor3 = face[jvert-1][iface];
      for ( i = 0; i < 3; i++ )
      {
        bytes_num = bytes_num + float_write ( fileout, cor3[i][icor3] );
      }

      icor3 = face[jvert][iface];
      for ( i = 0; i < 3; i++ )
      {
        bytes_num = bytes_num + float_write ( fileout, cor3[i][icor3] );
      }

      bytes_num = bytes_num + short_int_write ( fileout, attribute );

    }

  }
//
//  Report.
//
  cout << "\n";
  cout << "STLB_WRITE - Wrote " << bytes_num << " bytes.\n";

  if ( face_num != face_num2 )
  {
    cout << "  Number of faces in original data was " << face_num << ".\n";
    cout << "  Number of triangular faces in decomposed data is "
         << face_num2 << ".\n";
  }

  return 0;
}
//****************************************************************************80

void tds_pre_process ( )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_PRE_PROCESS divides the monolithic object into acceptably small pieces.
//
//  Discussion:
//
//    The 3DS binary format allows an unsigned short int for the number of
//    points, and number of faces in an object.  This limits such quantities
//    to 65535.  We have at least one interesting object with more faces
//    than that.  So we need to tag faces and nodes somehow.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    14 October 1998
//
//  Author:
//
//    John Burkardt
//
{
//
//  COMMENTED OUT
//
//  static unsigned short int BIG = 60000;
//

  return;
}
//****************************************************************************80

int tds_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_READ reads a 3D Studio MAX binary 3DS file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    20 October 1998
//
//  Author:
//
//    John Burkardt
//
{
  unsigned long int chunk_begin;
  unsigned long int   chunk_end;
  unsigned long int   chunk_length;
  unsigned long int   chunk_length2;
  unsigned long int   position;
  unsigned short int  temp_int;
  int                 version;
  int                 views_read;
//
//  Initialize.
//
  views_read = 0;

  temp_int = tds_read_u_short_int ( filein );

  if ( temp_int == 0x4d4d )
  {
    if ( debug )
    {
      cout << "TDS_READ: DEBUG: Read magic number "
           << hex << temp_int << "\n";
    }
//
//  Move to 28 bytes from the beginning of the file.
//
    position = 28;
    fseek ( filein, ( long ) position, SEEK_SET );
    version = fgetc ( filein );

    if ( version < 3 )
    {
      cout << "\n";
      cout << "TDS_READ - Fatal error!\n";
      cout << "  This routine can only read 3DS version 3 or later.\n";
      cout << "  The input file is version " << version << ".\n";
      return 1;
    }

    if ( debug )
    {
      cout << "TDS_READ: DEBUG: Version number is " << version << ".\n";
    }
//
//  Move to 2 bytes from the beginning of the file.
//  Set CURRENT_POINTER to the first byte of the chunk.
//  Set CHUNK_LENGTH to the number of bytes in the chunk.
//
    chunk_begin = 0;
    position = 2;
    fseek ( filein, ( long ) position, SEEK_SET );

    chunk_length = tds_read_u_long_int ( filein );
    position = 6;

    chunk_end = chunk_begin + chunk_length;

    if ( debug )
    {
      cout << "\n";
      cout << "TDS_READ:\n";
      cout << "  Chunk begin  = " << chunk_begin << "\n";
      cout << "  Chunk length = " << chunk_length << "\n";
      cout << "  Chunk end    = " << chunk_end << "\n";
    }

    while ( position + 2 < chunk_end )
    {
      temp_int = tds_read_u_short_int ( filein );
      position = position + 2;

      if ( debug )
      {
        cout << "TDS_READ: Short int = " << hex << temp_int << "\n";
        cout << "  position = " << position << "\n";
      }

      if ( temp_int == 0x0002 )
      {
        if ( debug )
        {
          cout << "TDS_READ: Read_Initial_Section:\n";
        }
        chunk_length2 = tds_read_u_long_int ( filein );
        position = position + 4;
        position = position - 6 + chunk_length2;
        fseek ( filein, ( long ) position, SEEK_SET );
      }
      else if ( temp_int == 0x3d3d )
      {
        if ( debug )
        {
          cout << "TDS_READ: Read_Edit_Section:\n";
        }
        position = position - 2;
        position = position + tds_read_edit_section ( filein, &views_read );
      }
      else if ( temp_int == 0xb000 )
      {
        if ( debug )
        {
          cout << "TDS_READ: Read_Keyframe_Section:\n";
        }

        position = position - 2;
        position = position + tds_read_keyframe_section ( filein, &views_read );
      }
      else
      {
        cout << "\n";
        cout << "TDS_READ - Fatal error!\n";
        cout << "  Unexpected input, position = " << position << "\n";
        cout << "  TEMP_INT = " << hex << temp_int << "\n";
        return 1;
      }
    }
    position = chunk_begin + chunk_length;
    fseek ( filein, ( long ) position, SEEK_SET );
  }
  else
  {
    cout << "\n";
    cout << "TDS_READ - Fatal error!\n";
    cout << "  Could not find the main section tag.\n";
    return 1;
  }

  return 0;
}
//****************************************************************************80

unsigned long tds_read_ambient_section ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_READ_AMBIENT_SECTION ???
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
{
  unsigned long int current_pointer;
  bool end_found = false;
  int i;
  long int pointer;
  float rgb_val[3];
  unsigned short int temp_int;
  unsigned long int temp_pointer;
  unsigned long int teller;
  unsigned char true_c_val[3];

  current_pointer = ftell ( filein ) - 2;
  temp_pointer = tds_read_u_long_int ( filein );
  teller = 6;

  while ( !end_found )
  {
    temp_int = tds_read_u_short_int ( filein );
    teller = teller + 2;

    switch ( temp_int )
    {
      case 0x0010:
        if ( debug )
        {
          cout << "     COLOR_F color definition section tag of "
            << hex << temp_int << "\n";
        }
        for ( i = 0; i < 3; i++ )
        {
          rgb_val[i] = float_read ( filein );
        }
        if ( debug )
        {
          cout << "RGB_VAL = "
            << setw(10) << rgb_val[0] << "  "
            << setw(10) << rgb_val[1] << "  "
            << setw(10) << rgb_val[2] << "\n";
        }
        teller = teller + 3 * sizeof ( float );
        break;
      case 0x0011:
        if ( debug )
        {
          cout << "     COLOR_24 24 bit color definition section tag of "
            << hex << temp_int << "\n";;
        }

        for ( i = 0; i < 3; i++ )
        {
          true_c_val[i] = fgetc ( filein );
        }
        if ( debug )
        {
          cout << "TRUE_C_VAL = "
            << setw(6) << true_c_val[0] << "  "
            << setw(6) << true_c_val[1] << "  "
            << setw(6) << true_c_val[2] << "\n";
        }
        teller = teller + 3;
        break;
      default:
        break;
    }

    if ( temp_pointer <= teller )
    {
      end_found = true;
    }

  }

  pointer = ( long ) ( current_pointer + temp_pointer );
  fseek ( filein, pointer, SEEK_SET );

  return ( temp_pointer );
}
//****************************************************************************80

unsigned long tds_read_background_section ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_READ_BACKGROUND_SECTION ???
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
{
  unsigned long int   current_pointer;
  bool end_found = false;
  int i;
  long int pointer;
  float rgb_val[3];
  unsigned short int temp_int;
  unsigned long int temp_pointer;
  unsigned long int teller;
  unsigned char true_c_val[3];

  current_pointer = ftell ( filein ) - 2;
  temp_pointer = tds_read_u_long_int ( filein );
  teller = 6;

  while ( !end_found )
  {
    temp_int = tds_read_u_short_int ( filein );
    teller = teller + 2;

    switch ( temp_int )
    {
      case 0x0010:
        if ( debug )
        {
          cout << "   COLOR_F RGB color definition section tag of "
            << hex << temp_int << "\n";
        }
        for ( i = 0; i < 3; i++ )
        {
          rgb_val[i] = float_read ( filein );
        }
        if ( debug )
        {
          cout << "RGB_VAL = "
               << setw(10) << rgb_val[0] << "  "
               << setw(10) << rgb_val[1] << "  "
               << setw(10) << rgb_val[2] << "\n";
        }
        teller = teller + 3 * sizeof ( float );
        break;
      case 0x0011:
        if ( debug )
        {
          cout << "   COLOR_24 24 bit color definition section tag of "
            << hex << temp_int << "\n";
        }

        for ( i = 0; i < 3; i++ )
        {
          true_c_val[i] = fgetc ( filein );
        }
        if ( debug )
        {
          cout << "TRUE_C_VAL = "
               << setw(6) << true_c_val[0] << "  "
               << setw(6) << true_c_val[1] << "  "
               << setw(6) << true_c_val[2] << "\n";
        }
        teller = teller + 3;
        break;
      default:
        break;
    }

    if ( temp_pointer <= teller )
    {
      end_found = true;
    }

  }

  pointer = ( long ) ( current_pointer + temp_pointer );
  fseek ( filein, pointer, SEEK_SET );

  return ( temp_pointer );
}
//****************************************************************************80

unsigned long tds_read_boolean ( unsigned char *boolean, FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_READ_BOOLEAN ???
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
{
  unsigned long current_pointer;
  long int pointer;
  unsigned long temp_pointer;

  current_pointer = ftell ( filein ) - 2;
  temp_pointer = tds_read_u_long_int ( filein );

  *boolean = fgetc ( filein );

  pointer = ( long ) ( current_pointer + temp_pointer );
  fseek ( filein, pointer, SEEK_SET );

  return ( temp_pointer );
}
//****************************************************************************80

unsigned long tds_read_camera_section ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_READ_CAMERA_SECTION ???
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
{
  float               camera_eye[3];
  float               camera_focus[3];
  unsigned long int   current_pointer;
  float               lens;
  long int            pointer;
  float               rotation;
  unsigned long int   temp_pointer;
  unsigned short int  u_short_int_val;

  current_pointer = ftell ( filein ) - 2;
  temp_pointer = tds_read_u_long_int ( filein );

  camera_eye[0] = float_read ( filein );
  camera_eye[1] = float_read ( filein );
  camera_eye[2] = float_read ( filein );

  camera_focus[0] = float_read ( filein );
  camera_focus[1] = float_read ( filein );
  camera_focus[2] = float_read ( filein );

  rotation = float_read ( filein );
  lens = float_read ( filein );

  if ( debug )
  {
    cout << " Found camera viewpoint at XYZ = "
      << setw(10) << camera_eye[0] << "  "
      << setw(10) << camera_eye[1] << "  "
      << setw(10) << camera_eye[2] << "\n";
    cout << "     Found camera focus coordinates at XYZ = "
      << setw(10) << camera_focus[0] << "  "
      << setw(10) << camera_focus[1] << "  "
      << setw(10) << camera_focus[2] << "\n";
    cout << "     Rotation of camera is:  " << rotation << "\n";
    cout << "     Lens in used camera is: " << lens << " mm.\n";
  }

  if ( 0 < ( temp_pointer-38 ) )
  {
    if ( debug )
    {
      cout << "          Found extra camera sections.\n";
    }

    u_short_int_val = tds_read_u_short_int ( filein );

    if ( u_short_int_val == 0x4710 )
    {
      if ( debug )
      {
        cout << "          CAM_SEE_CONE.\n";
      }
      tds_read_unknown_section ( filein );
    }

    u_short_int_val = tds_read_u_short_int ( filein );

    if ( u_short_int_val == 0x4720 )
    {
      if ( debug )
      {
        cout << "          CAM_RANGES.\n";
      }
      tds_read_unknown_section ( filein );
    }

  }

  pointer = ( long ) ( current_pointer + temp_pointer );
  fseek ( filein, pointer, SEEK_SET );

  return ( temp_pointer );
}
//****************************************************************************80

unsigned long tds_read_edit_section ( FILE *filein, int *views_read )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_READ_EDIT_SECTION ???
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    18 September 1998
//
{
  unsigned long int   chunk_length;
  unsigned long int   current_pointer;
  bool end_found = false;
  long int            pointer;
  unsigned long int   teller;
  unsigned short int  temp_int;

  current_pointer = ftell ( filein ) - 2;
  chunk_length = tds_read_u_long_int ( filein );
  teller = 6;

  while ( !end_found )
  {
    temp_int = tds_read_u_short_int ( filein );
    teller = teller + 2;

    if ( debug )
    {
      cout << "    TDS_READ_EDIT_SECTION processing tag "
           << hex << temp_int << "\n";
    }

    switch ( temp_int )
    {
      case 0x1100:
        if ( debug )
        {
          cout << "    BIT_MAP section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0x1201:
        if ( debug )
        {
          cout << "    USE_SOLID_BGND section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0x1300:
        if ( debug )
        {
          cout << "    V_GRADIENT section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0x1400:
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0x1420:
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0x1450:
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0x1500:
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0x2200:
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0x2201:
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0x2210:
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0x2300:
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0x2302:
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0x3000:
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0x2100:
        if ( debug )
        {
          cout << "    AMBIENT_LIGHT section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_ambient_section ( filein );
        break;
      case 0x1200:
        if ( debug )
        {
          cout << "    SOLID_BGND section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_background_section ( filein );
        break;
      case 0x0100:
        if ( debug )
        {
          cout << "    MASTER_SCALE section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0x3d3e:
        if ( debug )
        {
          cout << "    MESH_VERSION section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xafff:
        if ( debug )
        {
          cout << "    MAT_ENTRY section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_material_section ( filein );
        break;
      case 0x4000:
        if ( debug )
        {
          cout << "    NAMED_OBJECT section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_object_section ( filein );
        break;
      case 0x7001:
        if ( debug )
        {
          cout << "    VIEWPORT_LAYOUT section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_view_section ( filein, views_read );
        break;
      case 0x7012:
        if ( debug )
        {
          cout << "    VIEWPORT_DATA_3 section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0x7011:
        if ( debug )
        {
          cout << "    VIEWPORT_DATA section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0x7020:
        if ( debug )
        {
          cout << "    VIEWPORT_SIZE section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      default:
        if ( debug )
        {
          cout << "    Junk.\n";
        }
        break;
    }

    if ( chunk_length <= teller )
    {
      end_found = true;
    }

  }

  pointer = ( long ) ( current_pointer + chunk_length );

  fseek ( filein, pointer, SEEK_SET );

  return ( chunk_length );
}
//****************************************************************************80

unsigned long tds_read_keyframe_section ( FILE *filein, int *views_read )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_READ_KEYFRAME_SECTION ???
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
{
  unsigned long int   current_pointer;
  bool end_found = false;
  long int            pointer;
  unsigned short int  temp_int;
  unsigned long int   temp_pointer;
  unsigned long int   teller;

  current_pointer = ftell ( filein ) - 2;
  temp_pointer = tds_read_u_long_int ( filein );
  teller = 6;

  while ( !end_found )
  {
    temp_int = tds_read_u_short_int ( filein );
    teller = teller + 2;

    switch ( temp_int )
    {
      case 0x7001:
        if ( debug )
        {
          cout << "    VIEWPORT_LAYOUT main definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_view_section ( filein, views_read );
        break;
      case 0xb008:
        if ( debug )
        {
          cout << "    KFSEG frames section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xb002:
        if ( debug )
        {
          cout << "    OBJECT_NODE_TAG object description section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_keyframe_objdes_section ( filein );
        break;
      case 0xb009:
        if ( debug )
        {
          cout << "    KFCURTIME section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xb00a:
        if ( debug )
        {
          cout << "    KFHDR section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      default:
        break;
    }

    if ( temp_pointer <= teller )
    {
      end_found = true;
    }

  }

  pointer = ( long ) ( current_pointer + temp_pointer );
  fseek ( filein, pointer, SEEK_SET );

  return ( temp_pointer );
}
//****************************************************************************80

unsigned long tds_read_keyframe_objdes_section ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_READ_KEYFRAME_OBJDES_SECTION ???
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    21 September 1998
//
{
  unsigned long int   chunk_size;
  unsigned long int   current_pointer;
  bool end_found = false;
  long int            pointer;
  unsigned short int  temp_int;
  unsigned long int   temp_pointer;
  unsigned long int   teller;
  unsigned long int   u_long_int_val;
  unsigned short int  u_short_int_val;

  current_pointer = ftell ( filein ) - 2;
  temp_pointer = tds_read_u_long_int ( filein );
  teller = 6;

  while ( !end_found )
  {
    temp_int = tds_read_u_short_int ( filein );
    teller = teller + 2;

    switch ( temp_int )
    {
      case 0xb011:
        if ( debug )
        {
          cout << "      INSTANCE_NAME section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xb010:
        if ( debug )
        {
          cout << "      NODE_HDR section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xb020:
        if ( debug )
        {
          cout << "      POS_TRACK_TAG section tag of "
               << hex << temp_int << "\n";
        }
        chunk_size = tds_read_u_long_int ( filein );
        if ( debug )
        {
          cout << "      chunk_size = " << chunk_size << "\n";
        }
        u_short_int_val = tds_read_u_short_int ( filein );
        u_short_int_val = tds_read_u_short_int ( filein );
        u_short_int_val = tds_read_u_short_int ( filein );
        u_short_int_val = tds_read_u_short_int ( filein );
        u_short_int_val = tds_read_u_short_int ( filein );
        u_short_int_val = tds_read_u_short_int ( filein );
        u_short_int_val = tds_read_u_short_int ( filein );
        u_short_int_val = tds_read_u_short_int ( filein );
        u_long_int_val = tds_read_u_long_int ( filein );
        if ( debug )
        {
          cout << "u_short_int_val = " << u_short_int_val << "\n";
          cout << "u_long_int_val = " << u_long_int_val << "\n";
        }
        origin[0] = float_read ( filein );
        origin[1] = float_read ( filein );
        origin[2] = float_read ( filein );
        teller = teller + 32;
        break;
      case 0xb013:
        if ( debug )
        {
          cout << "      PIVOT section tag of "
               << hex << temp_int << "\n";
        }
        chunk_size = tds_read_u_long_int ( filein );
        pivot[0] = float_read ( filein );
        pivot[1] = float_read ( filein );
        pivot[2] = float_read ( filein );
        teller = teller + 12;
        break;
      case 0xb014:
        if ( debug )
        {
          cout << "      BOUNDBOX section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xb015:
        if ( debug )
        {
          cout << "      MORPH_SMOOTH section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xb021:
        if ( debug )
        {
          cout << "      ROT_TRACK_TAG section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xb022:
        if ( debug )
        {
          cout << "      SCL_TRACK_TAG section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xb030:
        if ( debug )
        {
          cout << "      NODE_ID section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      default:
        break;
    }

    if ( temp_pointer <= teller )
    {
      end_found = true;
    }

  }

  pointer = ( long ) ( current_pointer+temp_pointer );
  fseek ( filein, pointer, SEEK_SET );

  return ( temp_pointer );
}
//****************************************************************************80

unsigned long tds_read_light_section ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_READ_LIGHT_SECTION ???
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
{
  unsigned char boolean;
  unsigned long int current_pointer;
  bool end_found = false;
  int i;
  float light_coors[3];
  long int pointer;
  float rgb_val[3];
  unsigned long int teller;
  unsigned short int temp_int;
  unsigned long int temp_pointer;
  unsigned char true_c_val[3];

  current_pointer = ftell ( filein ) - 2;
  temp_pointer = tds_read_u_long_int ( filein );
  teller = 6;

  light_coors[0] = float_read ( filein );
  light_coors[1] = float_read ( filein );
  light_coors[2] = float_read ( filein );

  teller = teller + 3 * 4;

  if ( debug )
  {
    cout << "     Found light at coordinates XYZ = "
      << setw(10) << light_coors[0] << "  "
      << setw(10) << light_coors[1] << "  "
      << setw(10) << light_coors[2] << "\n";
  }

  while ( !end_found )
  {
    temp_int = tds_read_u_short_int ( filein );
    teller = teller + 2;

    switch ( temp_int )
    {
      case 0x0010:
        if ( debug )
        {
          cout << "      COLOR_F RGB color definition section tag of "
               << hex << temp_int << "\n";
        }
        for ( i = 0; i < 3; i++ )
        {
          rgb_val[i] = float_read ( filein );
        }
        if ( debug )
        {
          cout << "      RGB_VAL value set to "
            << setw(10) << rgb_val[0] << "  "
            << setw(10) << rgb_val[1] << "  "
            << setw(10) << rgb_val[2] << "\n";
        }
        teller = teller + 3 * sizeof ( float );
        break;
      case 0x0011:
        if ( debug )
        {
          cout << "      COLOR_24 24 bit color definition section tag of "
               << hex << temp_int << "\n";
        }

        for ( i = 0; i < 3; i++ )
        {
          true_c_val[i] = fgetc ( filein );
        }
        if ( debug )
        {
          cout << "      TRUE_C_VAL value set to "
            << setw(6) << true_c_val[0] << "  "
            << setw(6) << true_c_val[1] << "  "
            << setw(6) << true_c_val[2] << "\n";
        }
        teller = teller + 3;
        break;
      case 0x4620:
        if ( debug )
        {
          cout << "      DL_OFF section: "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_boolean ( &boolean, filein );
        if ( debug )
        {
          if ( boolean == 1 )
          {
            cout << "      Light is on.\n";
          }
          else
          {
            cout << "      Light is off.\n";
          }
        }
        break;
      case 0x4610:
        if ( debug  )
        {
          cout << "      DL_SPOTLIGHT section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_spot_section ( filein );
        break;
      case 0x465a:
        if ( debug  )
        {
          cout << "      DL_OUTER_RANGE section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      default:
        break;
    }

    if ( temp_pointer <= teller )
    {
      end_found = true;
    }

  }

  pointer = ( long ) ( current_pointer + temp_pointer );
  fseek ( filein, pointer, SEEK_SET );

  return ( temp_pointer );
}
//****************************************************************************80

unsigned long int tds_read_u_long_int ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_READ_U_LONG_INT ???
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    01 October 1998
//
//  Author:
//
//    John Burkardt
//
{
  union {
    unsigned long int yint;
    char ychar[4];
  } y;

  if ( byte_swap )
  {
    y.ychar[3] = fgetc ( filein );
    y.ychar[2] = fgetc ( filein );
    y.ychar[1] = fgetc ( filein );
    y.ychar[0] = fgetc ( filein );
  }
  else
  {
    y.ychar[0] = fgetc ( filein );
    y.ychar[1] = fgetc ( filein );
    y.ychar[2] = fgetc ( filein );
    y.ychar[3] = fgetc ( filein );
  }

  return y.yint;
}
//****************************************************************************80

int tds_read_long_name ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_READ_LONG_NAME ???
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
{
  unsigned char  letter;
  unsigned int   teller;

  teller = 0;
  letter = fgetc ( filein );
//
//  Could be a dummy object.
//
  if ( letter == 0 )
  {
    strcpy ( temp_name, "Default_name" );
    return -1;
  }

  temp_name[teller] = letter;
  teller = teller + 1;

  do
  {
    letter = fgetc ( filein );
    temp_name[teller] = letter;
    teller = teller + 1;
  } while ( letter != 0 );

  temp_name[teller-1] = 0;

  if ( debug )
  {
    cout << "      TDS_READ_LONG_NAME found name: " << temp_name << ".\n";
  }

  return teller;
}
//****************************************************************************80

unsigned long tds_read_matdef_section ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_READ_MATDEF_SECTION ???
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
{
  unsigned long int  current_pointer;
  long int           pointer;
  int                teller;
  unsigned long int  temp_pointer;

  current_pointer = ftell ( filein ) - 2;
  temp_pointer = tds_read_u_long_int ( filein );

  teller = tds_read_long_name ( filein );

  if ( teller == -1 )
  {
    if ( debug )
    {
      cout << "      No material name found.\n";
    }
  }
  else
  {
    strcpy ( mat_name, temp_name );
    if ( debug )
    {
      cout << "      Material name " << mat_name << ".\n";
    }
  }

  pointer = ( long ) ( current_pointer + temp_pointer );
  fseek ( filein, pointer, SEEK_SET );

  return ( temp_pointer );
}
//****************************************************************************80

unsigned long tds_read_material_section ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_READ_MATERIAL_SECTION ???
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
{
  unsigned long int   current_pointer;
  bool end_found = false;
  long int            pointer;
  unsigned short int  temp_int;
  unsigned long int   temp_pointer;
  unsigned long int   teller;

  current_pointer = ftell ( filein ) - 2;

  temp_pointer = tds_read_u_long_int ( filein );
  teller = 6;

  while ( !end_found )
  {
    temp_int = tds_read_u_short_int ( filein );
    teller = teller + 2;

    switch ( temp_int )
    {
      case 0xa000:
        if ( debug )
        {
          cout << "     MAT_NAME definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_matdef_section ( filein );
        break;
      case 0xa010:
        if ( debug )
        {
          cout << "     MAT_AMBIENT definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa020:
        if ( debug )
        {
          cout << "     MAT_DIFFUSE definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa030:
        if ( debug )
        {
          cout << "     MAT_SPECULAR definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa040:
        if ( debug )
        {
          cout << "     MAT_SHININESS definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa041:
        if ( debug )
        {
          cout << "     MAT_SHIN2PCT definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa042:
        if ( debug )
        {
          cout << "     MAT_SHIN3PCT definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa050:
        if ( debug )
        {
          cout << "     MAT_TRANSPARENCY definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa052:
        if ( debug )
        {
         cout << "     MAT_XPFALL definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa053:
        if ( debug )
        {
          cout << "     MAT_REFBLUR definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa080:
        if ( debug )
        {
          cout << "     MAT_SELF_ILLUM definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa081:
        if ( debug )
        {
          cout << "     MAT_TWO_SIDE definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa082:
        if ( debug )
        {
          cout << "     MAT_DECAL definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa083:
        if ( debug )
        {
          cout << "     MAT_ADDITIVE definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa084:
        if ( debug )
        {
          cout << "     MAT_SELF_ILPCT definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa085:
        if ( debug )
        {
          cout << "     MAT_WIRE definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa086:
        if ( debug )
        {
          cout << "     MAT_SUPERSMP definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa087:
        if ( debug )
        {
          cout << "     MAT_WIRESIZE definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa088:
        if ( debug )
        {
          cout << "     MAT_FACEMAP definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa08a:
        if ( debug )
        {
          cout << "     MAT_XPFALLIN definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa08c:
        if ( debug )
        {
          cout << "     MAT_PHONGSOFT definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa08e:
        if ( debug )
        {
          cout << "     MAT_WIREABS definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa100:
        if ( debug )
        {
          cout << "     MAT_SHADING definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa200:
        if ( debug )
        {
          cout << "     MAT_TEXMAP definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_texmap_section ( filein );
//
//        teller = teller + tds_read_unknown_section ( filein );
//
        break;
      case 0xa204:
        if ( debug )
        {
          cout << "     MAT_SPECMAP definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa210:
        if ( debug )
        {
          cout << "     MAT_OPACMAP definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa220:
        if ( debug )
        {
          cout << "     MAT_REFLMAP definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa230:
        if ( debug )
        {
          cout << "     MAT_BUMPMAP definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0xa353:
        if ( debug )
        {
          cout << "     MAT_MAP_TEXBLUR definition section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      default:
        if ( debug )
        {
          cout << "  Junk section tag of "
               << hex << temp_int << "\n";
        }
        break;
    }

    if ( temp_pointer <= teller )
    {
      end_found = true;
    }

  }
  pointer = ( long ) ( current_pointer + temp_pointer );

  fseek ( filein, pointer, SEEK_SET );

  return ( temp_pointer );
}
//****************************************************************************80

int tds_read_name ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_READ_NAME ???
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
{
  unsigned char  letter;
  unsigned int   teller;

  teller = 0;
  letter = fgetc ( filein );
//
//  Could be a dummy object.
//

  if ( letter == 0 )
  {
    strcpy ( temp_name, "Default name" );
    return (-1);
  }

  temp_name[teller] = letter;
  teller = teller + 1;

  do
  {
    letter = fgetc ( filein );
    temp_name[teller] = letter;
    teller = teller + 1;
  } while ( ( letter != 0 ) && ( teller < 12 ) );

  temp_name[teller-1] = 0;

  if ( debug )
  {
    cout << "      TDS_READ_NAME found name: " << temp_name << ".\n";
  }

  return 0;
}
//****************************************************************************80

unsigned long tds_read_obj_section ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_READ_OBJ_SECTION ???
//
//  Discussion:
//
//    Thanks to John F Flanagan for some suggested corrections.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    30 June 2001
//
{
  unsigned short int b;
  unsigned long int chunk_size;
  unsigned short int color_index;
  unsigned long int current_pointer;
  bool end_found = false;
  unsigned short int  g;
  int                 i;
  int                 j;
  int                 cor3_num_base;
  int                 cor3_num_inc;
  int                 face_num_inc;
  long int            pointer;
  unsigned short int  r;
  unsigned short int  temp_int;
  unsigned long int   temp_pointer;
  unsigned long int   temp_pointer2;
  unsigned long int   teller;

  current_pointer = ftell ( filein ) - 2;
  temp_pointer = tds_read_u_long_int ( filein );
  teller = 6;
  cor3_num_base = cor3_num;

  while ( !end_found )
  {
    temp_int = tds_read_u_short_int ( filein );
    teller = teller + 2;

    switch ( temp_int )
    {
      case 0x4000:
        if ( debug )
        {
          cout << "        NAMED_OBJECT section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;

      case 0x4100:
        if ( debug )
        {
          cout << "        N_TRI_OBJECT section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;

      case 0x4110:

        if ( debug )
        {
          cout << "        POINT_ARRAY section tag of "
               << hex << temp_int << "\n";
        }

        current_pointer = ftell ( filein ) - 2;
        temp_pointer2 = tds_read_u_long_int ( filein );
        cor3_num_inc =  ( int ) tds_read_u_short_int ( filein );

        for ( i = cor3_num; i < cor3_num + cor3_num_inc; i++ )
        {
          cor3[0][i] = float_read ( filein );
          cor3[1][i] = float_read ( filein );
          cor3[2][i] = float_read ( filein );
        }

        cor3_num = cor3_num + cor3_num_inc;
        teller = teller + temp_pointer2;
        break;

      case 0x4111:
        if ( debug )
        {
          cout << "        POINT_FLAG_ARRAY faces (2) section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;

      case 0x4120:

        if ( debug )
        {
          cout << "        FACE_ARRAY section tag of "
               << hex << temp_int << "\n";
        }

        temp_pointer2 = tds_read_u_long_int ( filein );
        face_num_inc = ( int ) tds_read_u_short_int ( filein );

        for ( i = face_num; i < face_num + face_num_inc; i++ )
        {
          face[0][i] = tds_read_u_short_int ( filein ) + cor3_num_base;
          face[1][i] = tds_read_u_short_int ( filein ) + cor3_num_base;
          face[2][i] = tds_read_u_short_int ( filein ) + cor3_num_base;
          face_order[i] = 3;
          face_flags[i] = tds_read_u_short_int ( filein );
//
//  Color is given per face, and as 24 bit RGB data packed in one word.
//  Extract RGB from the word, and assign R / 255 to each vertex.
//
//  Just a guess, JVB, 30 June 2001.
//
          temp_int = face_flags[i] & 0x000F;
          r = ( temp_int & 0x0004 ) >> 2;
          g = ( temp_int & 0x0002 ) >> 1;
          b = ( temp_int & 0x0001 );

          for ( j = 0; j < 3; j++ )
          {
            vertex_rgb[0][j][i] = ( float ) r / 255.0;
            vertex_rgb[1][j][i] = ( float ) g / 255.0;
            vertex_rgb[2][j][i] = ( float ) b / 255.0;
          }

        }

        temp_int = tds_read_u_short_int ( filein );
        if ( temp_int == 0x4150 )
        {
          for ( i = face_num; i < face_num + face_num_inc; i++ )
          {
            face_smooth[i] = ( int ) tds_read_u_long_int ( filein )
              + cor3_num_base;
          }
        }
        face_num = face_num + face_num_inc;
        teller = ftell ( filein );
        break;

      case 0x4130:
        if ( debug )
        {
          cout << "        MSH_MAT_GROUP section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;

      case 0x4140:
        if ( debug )
        {
          cout << "        TEX_VERTS section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_tex_verts_section ( filein );
        break;

      case 0x4150:
        if ( debug )
        {
          cout << "        SMOOTH_GROUP section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;

      case 0x4160:

        if ( debug )
        {
          cout << "        MESH_MATRIX section tag of "
               << hex << temp_int << "\n";
        }

        tds_read_u_long_int ( filein );

        for ( j = 0; j < 4; j++ )
        {
          for ( i = 0; i < 3; i++ )
          {
            transform_matrix[j][i] = float_read ( filein );
          }
        }
        transform_matrix[0][3] = 0.0;
        transform_matrix[1][3] = 0.0;
        transform_matrix[2][3] = 0.0;
        transform_matrix[3][3] = 0.0;

        teller = teller + 12 * sizeof ( float );
        break;

      case 0x4165:

        if ( debug )
        {
          cout << "        MESH_COLOR section tag of "
               << hex << temp_int << "\n";
        }

        chunk_size = tds_read_u_long_int ( filein );

        if ( chunk_size == 7 )
        {
          color_index = fgetc ( filein );
          teller = teller + 5;
        }
        else
        {
          color_index = tds_read_u_short_int ( filein );
          teller = teller + 6;
        }
        if ( debug )
        {
          cout << "        Color index set to " << color_index << "\n";
        }
        break;

      case 0x4170:
        if ( debug )
        {
          cout << "        MESH_TEXTURE_INFO section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;

      default:
        if ( debug )
        {
          cout << "        JUNK section tag of "
               << hex << temp_int << "\n";
        }
        break;
    }

    if (  temp_pointer <= teller )
    {
      end_found = true;
    }

  }

  pointer = ( long int ) ( current_pointer + temp_pointer );
  fseek ( filein, pointer, SEEK_SET );

  return ( temp_pointer );
}
//****************************************************************************80

unsigned long tds_read_object_section ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_READ_OBJECT_SECTION ???
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
{
  bool end_found = false;
  unsigned long int   current_pointer;
  int                 int_val;
  long int            pointer;
  unsigned short int  temp_int;
  unsigned long int   temp_pointer;
  unsigned long int   teller;

  current_pointer = ftell ( filein ) - 2;
  temp_pointer = tds_read_u_long_int ( filein );
  teller = 6;
//
//  Why don't you read and save the name here?
//
  int_val = tds_read_name ( filein );

  if ( int_val == -1 )
  {
    if ( debug )
    {
      cout << "      Dummy Object found\n";
    }
  }
  else
  {
    strcpy ( object_name, temp_name );
  }

  while ( !end_found )
  {
    temp_int = tds_read_u_short_int ( filein );
    teller = teller + 2;

    switch ( temp_int )
    {
      case 0x4700:
        if ( debug )
        {
          cout << "      N_CAMERA section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_camera_section ( filein );
        break;
      case 0x4600:
        if ( debug )
        {
          cout << "      N_DIRECT_LIGHT section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_light_section ( filein );
        break;
      case 0x4100:
        if ( debug )
        {
          cout << "      OBJ_TRIMESH section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_obj_section ( filein );
        break;
      case 0x4010:
        if ( debug )
        {
          cout << "      OBJ_HIDDEN section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0x4012:
        if ( debug )
        {
          cout << "      OBJ_DOESNT_CAST section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      default:
        break;
    }

    if ( temp_pointer <= teller )
    {
      end_found = true;
    }

  }

  pointer = ( long ) ( current_pointer + temp_pointer );

  fseek ( filein, pointer, SEEK_SET );

  return ( temp_pointer );
}
//****************************************************************************80

unsigned long int tds_read_tex_verts_section ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_READ_TEX_VERTS_SECTION reads the texture vertex data.
//
//  Discussion:
//
//    The texture vertex data seems to be associated with nodes.  This routine
//    distributes that data to vertices (nodes as they make up a particular
//    face).
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    02 July 1999
//
//  Author:
//
//    John Burkardt
//
{
  unsigned long int  current_pointer;
  int                icor3;
  long int           pointer;
  unsigned long int  temp_pointer;
  unsigned short int n2;

  current_pointer = ftell ( filein ) - 2;
  temp_pointer = tds_read_u_long_int ( filein );

  pointer = ( long int ) ( current_pointer + temp_pointer );

  n2 = tds_read_u_short_int ( filein );

  for ( icor3 = 0; icor3 < n2; icor3++ )
  {
    cor3_tex_uv[0][icor3] = float_read ( filein );
    cor3_tex_uv[1][icor3] = float_read ( filein );
  }

  fseek ( filein, pointer, SEEK_SET );

  return ( temp_pointer );
}
//****************************************************************************80

unsigned long tds_read_texmap_section ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_READ_TEXMAP_SECTION tries to get the TEXMAP name from the TEXMAP section.
//
//  Discussion:
//
//    The code has room for lots of textures.  In this routine, we behave as
//    though there were only one, and we stick its name in the first name slot.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    30 June 1999
//
//  Author:
//
//    John Burkardt
//
{
  unsigned long int current_pointer;
  long int           pointer;
  int                teller;
  unsigned long int  temp_pointer;

  texture_num = texture_num + 1;

  current_pointer = ftell ( filein ) - 2;
  temp_pointer = tds_read_u_long_int ( filein );

  tds_read_u_short_int ( filein );
  tds_read_u_short_int ( filein );
  tds_read_u_short_int ( filein );
  tds_read_u_short_int ( filein );
//
//  This next short int should equal A300.
//
  tds_read_u_short_int ( filein );
  tds_read_u_long_int ( filein );
//
//  Now read the TEXMAP file name.
//
  teller = tds_read_long_name ( filein );

  if ( teller == -1 )
  {
    if ( debug )
    {
      cout << "      No TEXMAP name found.\n";
    }
  }
  else
  {
    strcpy ( texture_name[0], temp_name );
    if ( debug )
    {
      cout << "      TEXMAP name " << texture_name[0] << ".\n";
    }
  }

  pointer = ( long ) ( current_pointer + temp_pointer );
  fseek ( filein, pointer, SEEK_SET );

  return ( temp_pointer );
}
//****************************************************************************80

unsigned short int tds_read_u_short_int ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_READ_U_SHORT_INT ???
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
{
  unsigned char  c1;
  unsigned char  c2;
  short int      ival;

  c1 = fgetc ( filein );
  c2 = fgetc ( filein );

  ival = c1 | ( c2 << 8 );

  return ival;
}
//****************************************************************************80

unsigned long tds_read_spot_section ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_READ_SPOT_SECTION ???
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
{
  unsigned long int current_pointer;
  float              falloff;
  float              hotspot;
  long int           pointer;
  float              target[4];
  unsigned long int  temp_pointer;

  current_pointer = ftell ( filein ) - 2;
  temp_pointer = tds_read_u_long_int ( filein );

  target[0] = float_read ( filein );
  target[1] = float_read ( filein );
  target[2] = float_read ( filein );
  hotspot = float_read ( filein );
  falloff = float_read ( filein );

  if ( debug )
  {
    cout << "      The target of the spot is XYZ = "
      << setw(10) << target[0] << "  "
      << setw(10) << target[1] << "  "
      << setw(10) << target[2] << "\n";
    cout << "      The hotspot of this light is " << hotspot << ".\n";
    cout << "      The falloff of this light is " << falloff << ".\n";
  }

  pointer = ( long ) ( current_pointer + temp_pointer );

  fseek ( filein, pointer, SEEK_SET );

  return ( temp_pointer );
}
//****************************************************************************80

unsigned long int tds_read_unknown_section ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_READ_UNKNOWN_SECTION ???
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
{
  unsigned long int current_pointer;
  long int           pointer;
  unsigned long int  temp_pointer;

  current_pointer = ftell ( filein ) - 2;
  temp_pointer = tds_read_u_long_int ( filein );

  pointer = ( long int ) ( current_pointer + temp_pointer );

  fseek ( filein, pointer, SEEK_SET );

  return ( temp_pointer );
}
//****************************************************************************80

unsigned long tds_read_view_section ( FILE *filein, int *views_read )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_READ_VIEW_SECTION ???
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
{
  unsigned long int current_pointer;
  bool end_found = false;
  long int            pointer;
  unsigned short int  temp_int;
  unsigned long int   temp_pointer;
  unsigned long int   teller;

  current_pointer = ftell ( filein ) - 2;
  temp_pointer = tds_read_u_long_int ( filein );
  teller = 6;

  while ( !end_found )
  {
    temp_int = tds_read_u_short_int ( filein );
    teller = teller + 2;

    switch ( temp_int )
    {
      case 0x7012:
        if ( debug )
        {
          cout << "     VIEWPORT_DATA_3 section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_vp_section ( filein, views_read );
        break;
      case 0x7011:
        if ( debug )
        {
          cout << "     VIEWPORT_DATA section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_unknown_section ( filein );
        break;
      case 0x7020:
        if ( debug )
        {
          cout << "     VIEWPORT_SIZE section tag of "
               << hex << temp_int << "\n";
        }
        teller = teller + tds_read_vp_section ( filein, views_read );
        break;
      default:
        break;
    }

    if ( temp_pointer <= teller )
    {
      end_found = true;
    }

    if ( 3 < *views_read )
    {
      end_found = true;
    }
  }

  pointer = ( long int ) ( current_pointer + temp_pointer );

  fseek ( filein, pointer, SEEK_SET );

  return ( temp_pointer );
}
//****************************************************************************80

unsigned long tds_read_vp_section ( FILE *filein, int *views_read )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_READ_VP_SECTION ???
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
{
  unsigned int       attribs;
  unsigned long int  current_pointer;
  int                i;
  int                int_val;
  long int           pointer;
  unsigned int       port;
  unsigned long int  temp_pointer;
  char              *viewports[11] = {
                      "Bogus",
                      "Top",
                      "Bottom",
                      "Left",
                      "Right",
                      "Front",
                      "Back",
                      "User",
                      "Camera",
                      "Light",
                      "Disabled"
                     };

  *views_read = *views_read + 1;

  current_pointer = ftell ( filein ) - 2;
  temp_pointer = tds_read_u_long_int ( filein );

  attribs = tds_read_u_short_int ( filein );

  if ( attribs == 3 )
  {
    if ( debug )
    {
      cout << "<Snap> active in viewport.\n";
    }
  }

  if ( attribs == 5 )
  {
    if ( debug )
    {
      cout << "<Grid> active in viewport.\n";
    }
  }
//
//  Read 5 INTS to get to the viewport information.
//
  for ( i = 1; i < 6; i++ )
  {
    tds_read_u_short_int ( filein );
  }

  port = tds_read_u_short_int ( filein );
//
//  Find camera section.
//
  if ( ( port == 0xffff ) || ( port == 0 ) )
  {
    for ( i = 0; i < 12; i++ )
    {
      tds_read_u_short_int ( filein );
    }

    int_val = tds_read_name (filein );

    if ( int_val == -1 )
    {
      if ( debug )
      {
        cout << "      No Camera name found\n";;
      }
    }

    port = 0x0008;
  }

  if ( debug )
  {
    cout << "Reading [" << viewports[port]
         << "] information with tag: " << port << "\n";
  }

  pointer = ( long int ) ( current_pointer + temp_pointer );

  fseek ( filein, pointer, SEEK_SET );

  return ( temp_pointer );
}
//****************************************************************************80

int tds_write ( FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_WRITE writes graphics information to a 3D Studio Max 3DS file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    14 October 1998
//
//  Author:
//
//    John Burkardt
//
//
{
  float               float_val;
  int                 i;
  int                 icor3;
  int                 iface;
  int                 j;
  long int            l0002;
  long int            l0100;
  long int            l3d3d;
  long int            l3d3e;
  long int            l4000;
  long int            l4100;
  long int            l4110;
  long int            l4120;
  long int            l4150;
  long int            l4160;
  long int            l4d4d;
  long int            lb000;
  long int            lb002;
  long int            lb00a;
  long int            lb008;
  long int            lb009;
  long int            lb010;
  long int            lb013;
  long int            lb020;
  long int            lb021;
  long int            lb022;
  long int            lb030;
  long int            long_int_val;
  int                 name_length;
  short int           short_int_val;
  unsigned short int  u_short_int_val;

  bytes_num = 0;
  name_length = strlen ( object_name );

  l0002 = 10;

  l4150 = 2 + 4 + face_num * 4;
  l4120 = 2 + 4 + 2 + 4 * face_num * 2 + l4150;
  l4160 = 2 + 4 + 4 * 12;
  l4110 = 2 + 4 + 2 + cor3_num * 3 * 4;
  l4100 = 2 + 4 + l4110 + l4160 + l4120;
  l4000 = 2 + 4 + ( name_length + 1 ) + l4100;
  l0100 = 2 + 4 + 4;
  l3d3e = 2 + 4 + 4;
  l3d3d = 2 + 4 + l3d3e + l0100 + l4000;

  lb022 = 2 + 4 + 32;
  lb021 = 2 + 4 + 9 * 4;
  lb020 = 2 + 4 + 8 * 4;
  lb013 = 2 + 4 + 6 * 2;
  lb010 = 2 + 4 + ( name_length + 1 ) + 3 * 2;
  lb030 = 2 + 4 + 2;
  lb002 = 2 + 4 + lb030 + lb010 + lb013 + lb020 + lb021 + lb022;
  lb009 = 2 + 4 + 4;
  lb008 = 2 + 4 + 2 * 4;
  lb00a = 2 + 4 + 2 + 9 + 2 * 2;
  lb000 = 2 + 4 + lb00a + lb008 + lb009 + lb002;

  l4d4d = 2 + 4 + l0002 + l3d3d + lb000;
//
//  M3DMAGIC begin.
//    tag, size.
//
  short_int_val = ( short ) 0x4d4d;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  bytes_num = bytes_num + long_int_write ( fileout, l4d4d );
//
//  M3D_VERSION begin.
//    tag, size, version.
//
  short_int_val = ( short ) 0x0002;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  bytes_num = bytes_num + long_int_write ( fileout, l0002 );
  long_int_val = 3;
  bytes_num = bytes_num + long_int_write ( fileout, long_int_val );
//
//  M3D_VERSION end.
//  MDATA begin.
//    tag, size.
//
  short_int_val = ( short ) 0x3d3d;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  bytes_num = bytes_num + long_int_write ( fileout, l3d3d );
//
//  MESH_VERSION begin.
//    tag, size, version.
//
  short_int_val = ( short ) 0x3d3e;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  bytes_num = bytes_num + long_int_write ( fileout, l3d3e );
  long_int_val = 3;
  bytes_num = bytes_num + long_int_write ( fileout, long_int_val );
//
//  MESH_VERSION end.
//  MASTER_SCALE begin.
//    tag, size, scale.
//
  short_int_val = ( short ) 0x0100;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  bytes_num = bytes_num + long_int_write ( fileout, l0100 );
  float_val = 1.0;
  bytes_num = bytes_num + float_write ( fileout, float_val );
//
//  MASTER_SCALE end.
//  NAMED_OBJECT begin.
//    tag, size, name.
//
  short_int_val = ( short ) 0x4000;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  bytes_num = bytes_num + long_int_write ( fileout, l4000 );
  bytes_num = bytes_num + tds_write_string ( fileout, object_name );
//
//  N_TRI_OBJECT begin.
//    tag, size.
//
  short_int_val = ( short ) 0x4100;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  bytes_num = bytes_num + long_int_write ( fileout, l4100 );
//
//  POINT_ARRAY begin.
//    tag, size, number of points, coordinates of points.
//  Warning! number of points could exceed a short!
//
  short_int_val = ( short ) 0x4110;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  bytes_num = bytes_num + long_int_write ( fileout, l4110 );

  u_short_int_val = ( unsigned short ) cor3_num;
  bytes_num = bytes_num + tds_write_u_short_int ( fileout, u_short_int_val );

  for ( icor3 = 0; icor3 < cor3_num; icor3++ )
  {
    for ( j = 0; j < 3; j++ )
    {
      bytes_num = bytes_num + float_write ( fileout, cor3[j][icor3] );
    }
  }
//
//  POINT_ARRAY end.
//  MESH_MATRIX begin.
//    tag, size, 4 by 3 matrix.
//
  short_int_val = ( short ) 0x4160;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  bytes_num = bytes_num + long_int_write ( fileout, l4160 );

  for ( i = 0; i < 4; i++ )
  {
    for ( j = 0; j < 3; j++ )
    {
      float_val = transform_matrix[i][j];
      bytes_num = bytes_num + float_write ( fileout, float_val );
    }
  }
//
//  MESH_MATRIX end.
//  FACE_ARRAY begin.
//    tag, size, number of faces, nodes per face.
//  Warning: number of faces could exceed a short!
//
  short_int_val = ( short ) 0x4120;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  bytes_num = bytes_num + long_int_write ( fileout, l4120 );

  u_short_int_val = ( unsigned short ) face_num;
  bytes_num = bytes_num + tds_write_u_short_int ( fileout, u_short_int_val );

  for ( iface = 0; iface < face_num; iface++ )
  {
    for ( j = 0; j < 3; j++ )
    {
      short_int_val = face[j][iface];
      bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
    }
    short_int_val = face_flags[iface];
    bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  }
//
//  SMOOTH_GROUP begin.
//    tag, size, group for each face.
//
  short_int_val = ( short ) 0x4150;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  bytes_num = bytes_num + long_int_write ( fileout, l4150 );

  for ( iface = 0; iface < face_num; iface++ )
  {
    long_int_val = face_smooth[iface];
    bytes_num = bytes_num + long_int_write ( fileout, long_int_val );
  }
//
//  SMOOTH_GROUP end.
//  FACE_ARRAY end.
//  N_TRI_OBJECT end.
//  NAMED_OBJECT end.
//  MDATA end.
//  KFDATA begin.
//
  short_int_val = ( short ) 0xb000;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  bytes_num = bytes_num + long_int_write ( fileout, lb000 );
//
//  KFHDR begin.
//    tag, size, revision, filename, animlen.
//
  short_int_val = ( short ) 0xb00a;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  bytes_num = bytes_num + long_int_write ( fileout, lb00a );
  short_int_val = 5;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  bytes_num = bytes_num + tds_write_string ( fileout, "MAXSCENE" );
  short_int_val = 100;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  short_int_val = 0;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
//
//  KFHDR end.
//  KFSEG begin.
//    tag, size, start, end.
//
  short_int_val = ( short ) 0xb008;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  bytes_num = bytes_num + long_int_write ( fileout, lb008 );
  long_int_val = 0;
  bytes_num = bytes_num + long_int_write ( fileout, long_int_val );
  long_int_val = 100;
  bytes_num = bytes_num + long_int_write ( fileout, long_int_val );
//
//  KFSEG end.
//  KFCURTIME begin.
//    tag, size, current_frame.
//
  short_int_val = ( short ) 0xb009;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  bytes_num = bytes_num + long_int_write ( fileout, lb009 );
  long_int_val = 0;
  bytes_num = bytes_num + long_int_write ( fileout, long_int_val );
//
//  KFCURTIME end.
//  OBJECT_NODE_TAG begin.
//    tag, size.
//
  short_int_val = ( short ) 0xb002;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  bytes_num = bytes_num + long_int_write ( fileout, lb002 );
//
//  NODE_ID begin.
//    tag, size, id.
//
  short_int_val = ( short ) 0xb030;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  bytes_num = bytes_num + long_int_write ( fileout, lb030 );
  short_int_val = 0;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
//
//  NODE_ID end.
//  NODE_HDR begin.
//    tag, size, object_name, flag1, flag2, hierarchy.
//
  short_int_val = ( short ) 0xb010;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  bytes_num = bytes_num + long_int_write ( fileout, lb010 );
  bytes_num = bytes_num + tds_write_string ( fileout, object_name );
  short_int_val = 16384;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  short_int_val = 0;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  short_int_val = -1;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
//
//  NODE_HDR end.
//  PIVOT begin.
//    tag, size, pivot_x, pivot_y, pivot_z.
//
  short_int_val = ( short ) 0xb013;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  bytes_num = bytes_num + long_int_write ( fileout, lb013 );
  for ( i = 0; i < 3; i++ )
  {
    float_val = pivot[i];
    bytes_num = bytes_num + float_write ( fileout, float_val );
  }
//
//  PIVOT end.
//  POS_TRACK_TAG begin.
//    tag, size, flag, i1, i2, i3, i4, i5, i6, frame, l1, pos_x, pos_y, pos_z.
//
  short_int_val = ( short ) 0xb020;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  bytes_num = bytes_num + long_int_write ( fileout, lb020 );
  short_int_val = 0;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  short_int_val = 0;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  short_int_val = 0;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  short_int_val = 0;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  short_int_val = 0;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  short_int_val = 1;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  short_int_val = 0;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  short_int_val = 0;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  long_int_val = 0;
  bytes_num = bytes_num + long_int_write ( fileout, long_int_val );
  for ( i = 0; i < 3; i++ )
  {
    float_val = origin[i];
    bytes_num = bytes_num + float_write ( fileout, float_val );
  }
//
//  POS_TRACK_TAG end.
//  ROT_TRACK_TAG begin.
//    tag, size, i1, i2, i3, i4, i5, i6, i7, i8, l1, rad, axis_x, axis_y, axis_z.
//
  short_int_val = ( short ) 0xb021;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  bytes_num = bytes_num + long_int_write ( fileout, lb021 );
  short_int_val = 0;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  short_int_val = 0;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  short_int_val = 0;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  short_int_val = 0;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  short_int_val = 0;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  short_int_val = 1;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  short_int_val = 0;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  short_int_val = 0;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  long_int_val = 0;
  bytes_num = bytes_num + long_int_write ( fileout, long_int_val );
  float_val = 0.0;
  bytes_num = bytes_num + float_write ( fileout, float_val );
  bytes_num = bytes_num + float_write ( fileout, float_val );
  bytes_num = bytes_num + float_write ( fileout, float_val );
  bytes_num = bytes_num + float_write ( fileout, float_val );
//
//  ROT_TRACK_TAG end.
//  SCL_TRACK_TAG begin.
//    tag, size, i1, i2, i3, i4, i5, i6, i7, i8, l1, scale_x, scale_y, scale_z.
//
  short_int_val = ( short ) 0xb022;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  bytes_num = bytes_num + long_int_write ( fileout, lb022 );
  short_int_val = 0;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  short_int_val = 0;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  short_int_val = 0;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  short_int_val = 0;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  short_int_val = 0;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  short_int_val = 1;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  short_int_val = 0;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  short_int_val = 0;
  bytes_num = bytes_num + short_int_write ( fileout, short_int_val );
  long_int_val = 0;
  bytes_num = bytes_num + long_int_write ( fileout, long_int_val );
  float_val = 1.0;
  bytes_num = bytes_num + float_write ( fileout, float_val );
  bytes_num = bytes_num + float_write ( fileout, float_val );
  bytes_num = bytes_num + float_write ( fileout, float_val );
//
//  SCL_TRACK_TAG end.
//  OBJECT_NODE_TAG end.
//  KFDATA end.
//  M3DMAGIC end.
//

//
//  Report.
//
  cout << "\n";
  cout << "TDS_WRITE wrote " << bytes_num << " bytes.\n";

  return 0;
}
//****************************************************************************80

int tds_write_string ( FILE *fileout, char *string )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_WRITE_STRING ???
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    23 September 1998
//
//  Author:
//
//    John Burkardt
//
{
  char *c;
  int   nchar;

  nchar = 0;

  for ( c = string; nchar < 12; c++ )
  {
    fputc ( *c, fileout );
    nchar = nchar + 1;

    if  ( *c == 0 )
    {
      return nchar;
    }

  }

  return nchar;
}
//****************************************************************************80

int tds_write_u_short_int ( FILE *fileout, unsigned short int short_int_val )

//****************************************************************************80
//
//  Purpose:
//
//    TDS_WRITE_U_SHORT_INT ???
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    14 October 1998
//
//  Author:
//
//    John Burkardt
//
{
  union {
    unsigned short int yint;
    char ychar[2];
  } y;

  y.yint = short_int_val;

  if ( byte_swap )
  {
    fputc ( y.ychar[1], fileout );
    fputc ( y.ychar[0], fileout );
  }
  else
  {
    fputc ( y.ychar[0], fileout );
    fputc ( y.ychar[1], fileout );
  }

  return 2;
}
//****************************************************************************80

int tec_write ( FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    TEC_WRITE writes graphics information to a TECPLOT file.
//
//  Discussion:
//
//    The file format used is appropriate for 3D finite element surface
//    zone data.  Polygons are decomposed into triangles where necessary.
//
//  Example:
//
//    TITLE = "cube.tec created by IVCON."
//    VARIABLES = "X", "Y", "Z", "R", "G", "B"
//    ZONE T="TRIANGLES", N=8, E=12, F=FEPOINT, ET=TRIANGLE
//    0.0 0.0 0.0 0.0 0.0 0.0
//    1.0 0.0 0.0 1.0 0.0 0.0
//    1.0 1.0 0.0 1.0 1.0 0.0
//    0.0 1.0 0.0 0.0 1.0 0.0
//    0.0 0.0 1.0 0.0 0.0 1.0
//    1.0 0.0 1.0 1.0 0.0 1.0
//    1.0 1.0 1.0 1.0 1.0 1.0
//    0.0 1.0 1.0 0.0 1.0 1.0
//    1 4 2
//    2 4 3
//    1 5 8
//    1 2 5
//    2 6 5
//    2 3 6
//    3 7 6
//    3 4 7
//    4 8 7
//    4 1 8
//    5 6 8
//    6 7 8
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    09 June 1999
//
//  Author:
//
//    John Burkardt
//
{
  float b;
  int face2[3];
  float g;
  int icor3;
  int iface;
  int imat;
  int j;
  int face_num2;
  int text_num;
  float r;
//
//  Determine the number of triangular faces.
//
  face_num2 = 0;
  for ( iface = 0; iface < face_num; iface++ )
  {
    for ( j = 0; j < face_order[iface] - 2; j++ )
    {
      face_num2 = face_num2 + 1;
    }
  }

  text_num = 0;

  fprintf ( fileout, "TITLE = \"%s created by IVCON.\"\n", fileout_name );
  fprintf ( fileout, "VARIABLES = \"X\", \"Y\", \"Z\", \"R\", \"G\", \"B\"\n" );
  fprintf ( fileout,
    "ZONE T=\"TRIANGLES\", N=%d, E=%d, F=FEPOINT, ET=TRIANGLE\n",
    cor3_num, face_num2 );

  text_num = text_num + 3;
//
//  Write out X, Y, Z, R, G, B per node.
//
  for ( icor3 = 0; icor3 < cor3_num; icor3++ )
  {
    imat = cor3_material[icor3];
    r = material_rgba[0][imat];
    g = material_rgba[1][imat];
    b = material_rgba[2][imat];
    fprintf ( fileout, "%f %f %f %f %f %f\n", cor3[0][icor3], cor3[1][icor3],
      cor3[2][icor3], r, g, b );
    text_num = text_num + 1;
  }
//
//  Do the next face.
//
  for ( iface = 0; iface < face_num; iface++ )
  {
//
//  Break the face up into triangles, anchored at node 1.
//
    for ( j = 0; j < face_order[iface] - 2; j++ )
    {
      face2[0] = face[  0][iface] + 1;
      face2[1] = face[j+1][iface] + 1;
      face2[2] = face[j+2][iface] + 1;

      fprintf ( fileout, "%d %d %d\n", face2[0], face2[1], face2[2] );
      text_num = text_num + 1;

    }

  }
//
//  Report.
//
  cout << "\n";
  cout << "TEC_WRITE - Wrote " << text_num << " text lines.\n";

  return 0;
}
//****************************************************************************80

void tmat_init ( float a[4][4] )

//****************************************************************************80
//
//  Purpose:
//
//    TMAT_INIT initializes the geometric transformation matrix.
//
//  Discussion:
//
//    The geometric transformation matrix can be thought of as a 4 by 4
//    matrix "A" having components:
//
//      r11 r12 r13 t1
//      r21 r22 r23 t2
//      r31 r32 r33 t3
//        0   0   0  1
//
//    This matrix encodes the rotations, scalings and translations that
//    are applied to graphical objects.
//
//    A point P = (x,y,z) is rewritten in "homogeneous coordinates" as
//    PH = (x,y,z,1).  Then to apply the transformations encoded in A to
//    the point P, we simply compute A * PH.
//
//    Individual transformations, such as a scaling, can be represented
//    by simple versions of the transformation matrix.  If the matrix
//    A represents the current set of transformations, and we wish to
//    apply a new transformation B, { the original points are
//    transformed twice:  B * ( A * PH ).  The new transformation B can
//    be combined with the original one A, to give a single matrix C that
//    encodes both transformations: C = B * A.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Foley, van Dam, Feiner, Hughes,
//    Computer Graphics, Principles and Practice,
//    Addison Wesley, Second Edition, 1990.
//
//  Parameters:
//
//    Input, float A[4][4], the geometric transformation matrix.
//
{
  int i;
  int j;

  for ( i = 0; i < 4; i++ )
  {
    for ( j = 0; j < 4; j++ )
    {
      if ( i == j )
      {
        a[i][j] = 1.0;
      }
      else
      {
        a[i][j] = 0.0;
      }
    }
  }
  return;
}
//****************************************************************************80

void tmat_mxm ( float a[4][4], float b[4][4], float c[4][4] )

//****************************************************************************80
//
//  Purpose:
//
//    TMAT_MXM multiplies two geometric transformation matrices.
//
//  Discussion:
//
//    The product is accumulated in a temporary array, and { assigned
//    to the result.  Therefore, it is legal for any two, or all three,
//    of the arguments to share memory.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Foley, van Dam, Feiner, Hughes,
//    Computer Graphics, Principles and Practice,
//    Addison Wesley, Second Edition, 1990.
//
//  Parameters:
//
//    Input, float A[4][4], the first geometric transformation matrix.
//
//    Input, float B[4][4], the second geometric transformation matrix.
//
//    Output, float C[4][4], the product A * B.
//
{
  float d[4][4];
  int i;
  int j;
  int k;

  for ( i = 0; i < 4; i++ )
  {
    for ( k = 0; k < 4; k++ )
    {
      d[i][k] = 0.0;
      for ( j = 0; j < 4; j++ )
      {
        d[i][k] = d[i][k] + a[i][j] * b[j][k];
      }
    }
  }

  for ( i = 0; i < 4; i++ )
  {
    for ( j = 0; j < 4; j++ )
    {
      c[i][j] = d[i][j];
    }
  }
  return;
}
//****************************************************************************80

void tmat_mxp ( float a[4][4], float x[4], float y[4] )

//****************************************************************************80
//
//  Purpose:
//
//    TMAT_MXP multiplies a geometric transformation matrix times a point.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Foley, van Dam, Feiner, Hughes,
//    Computer Graphics, Principles and Practice,
//    Addison Wesley, Second Edition, 1990.
//
//  Parameters:
//
//    Input, float A[4][4], the geometric transformation matrix.
//
//    Input, float X[4], the point to be multiplied.  The fourth component
//    of X is implicitly assigned the value of 1.
//
//    Output, float Y[4], the result of A*X.  The product is accumulated in
//    a temporary vector, and { assigned to the result.  Therefore, it
//    is legal for X and Y to share memory.
//
{
  int i;
  int j;
  float z[4];

  for ( i = 0; i < 3; i++ )
  {
    z[i] = a[i][3];
    for ( j = 0; j < 3; j++ )
    {
      z[i] = z[i] + a[i][j] * x[j];
    }
  }

  for ( i = 0; i < 3; i++ )
  {
    y[i] = z[i];
  }
  return;
}
//****************************************************************************80

void tmat_mxp2 ( float a[4][4], float x[][3], float y[][3], int n )

//****************************************************************************80
//
//  Purpose:
//
//    TMAT_MXP2 multiplies a geometric transformation matrix times N points.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    20 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Foley, van Dam, Feiner, Hughes,
//    Computer Graphics, Principles and Practice,
//    Addison Wesley, Second Edition, 1990.
//
//  Parameters:
//
//    Input, float A[4][4], the geometric transformation matrix.
//
//    Input, float X[N][3], the points to be multiplied.
//
//    Output, float Y[N][3], the transformed points.  Each product is
//    accumulated in a temporary vector, and { assigned to the
//    result.  Therefore, it is legal for X and Y to share memory.
//
//
{
  int i;
  int j;
  int k;
  float z[4];

  for ( k = 0; k < n; k++ )
  {
    for ( i = 0; i < 3; i++ )
    {
      z[i] = a[i][3];
      for ( j = 0; j < 3; j++ )
      {
        z[i] = z[i] + a[i][j] * x[k][j];
      }
    }

    for ( i = 0; i < 3; i++ )
    {
      y[k][i] = z[i];
    }

  }
  return;
}
//****************************************************************************80

void tmat_mxv ( float a[4][4], float x[4], float y[4] )

//****************************************************************************80
//
//  Purpose:
//
//    TMAT_MXV multiplies a geometric transformation matrix times a vector.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    12 August 1999
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Foley, van Dam, Feiner, Hughes,
//    Computer Graphics, Principles and Practice,
//    Addison Wesley, Second Edition, 1990.
//
//  Parameters:
//
//    Input, float A[4][4], the geometric transformation matrix.
//
//    Input, float X[3], the vector to be multiplied.  The fourth component
//    of X is implicitly assigned the value of 1.
//
//    Output, float Y[3], the result of A*X.  The product is accumulated in
//    a temporary vector, and assigned to the result.  Therefore, it
//    is legal for X and Y to share memory.
//
{
  int i;
  int j;
  float z[4];

  for ( i = 0; i < 3; i++ )
  {
    z[i] = 0.0;
    for ( j = 0; j < 3; j++ )
    {
      z[i] = z[i] + a[i][j] * x[j];
    }
    z[i] = z[i] + a[i][3];
  }

  for ( i = 0; i < 3; i++ )
  {
    y[i] = z[i];
  }
  return;
}
//****************************************************************************80

void tmat_rot_axis ( float a[4][4], float b[4][4], float angle,
  char axis )

//****************************************************************************80
//
//  Purpose:
//
//    TMAT_ROT_AXIS applies an axis rotation to the geometric transformation matrix.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 April 1999
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Foley, van Dam, Feiner, Hughes,
//    Computer Graphics, Principles and Practice,
//    Addison Wesley, Second Edition, 1990.
//
//  Parameters:
//
//    Input, float A[4][4], the current geometric transformation matrix.
//
//    Output, float B[4][4], the modified geometric transformation matrix.
//    A and B may share the same memory.
//
//    Input, float ANGLE, the angle, in degrees, of the rotation.
//
//    Input, character AXIS, is 'X', 'Y' or 'Z', specifying the coordinate
//    axis about which the rotation occurs.
//
{
  float c[4][4];
  float d[4][4];
  int i;
  int j;
  float theta;

  theta = angle * DEG_TO_RAD;

  tmat_init ( c );

  if ( axis == 'X' || axis == 'x' )
  {
    c[1][1] =   cos ( theta );
    c[1][2] = - sin ( theta );
    c[2][1] =   sin ( theta );
    c[2][2] =   cos ( theta );
  }
  else if ( axis == 'Y' || axis == 'y' )
  {
    c[0][0] =   cos ( theta );
    c[0][2] =   sin ( theta );
    c[2][0] = - sin ( theta );
    c[2][2] =   cos ( theta );
  }
  else if ( axis == 'Z' || axis == 'z' )
  {
    c[0][0] =   cos ( theta );
    c[0][1] = - sin ( theta );
    c[1][0] =   sin ( theta );
    c[1][1] =   cos ( theta );
  }
  else
  {
    cout << "\n";
    cout << "TMAT_ROT_AXIS - Fatal error!\n";
    cout << "  Illegal rotation axis: " << axis << ".\n";
    cout << "  Legal choices are 'X', 'Y', or 'Z'.\n";
    return;
  }

  tmat_mxm ( c, a, d );

  for ( i = 0; i < 4; i++ )
  {
    for ( j = 0; j < 4; j++ )
    {
      b[i][j] = d[i][j];
    }
  }
  return;
}
//****************************************************************************80

void tmat_rot_vector ( float a[4][4], float b[4][4], float angle,
  float v1, float v2, float v3 )

//****************************************************************************80
//
//  Purpose:
//
//    TMAT_ROT_VECTOR applies a rotation about a vector to the geometric transformation matrix.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    27 July 1999
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Foley, van Dam, Feiner, Hughes,
//    Computer Graphics, Principles and Practice,
//    Addison Wesley, Second Edition, 1990.
//
//  Parameters:
//
//    Input, float A[4][4], the current geometric transformation matrix.
//
//    Output, float B[4][4], the modified geometric transformation matrix.
//    A and B may share the same memory.
//
//    Input, float ANGLE, the angle, in degrees, of the rotation.
//
//    Input, float V1, V2, V3, the X, Y and Z coordinates of a (nonzero)
//    point defining a vector from the origin.  The rotation will occur
//    about this axis.
//
{
  float c[4][4];
  float ca;
  float d[4][4];
  int i;
  int j;
  float sa;
  float theta;

  if ( v1 * v1 + v2 * v2 + v3 * v3 == 0.0 )
  {
    return;
  }

  theta = angle * DEG_TO_RAD;

  tmat_init ( c );

  ca = cos ( theta );
  sa = sin ( theta );

  c[0][0] =                v1 * v1 + ca * ( 1.0 - v1 * v1 );
  c[0][1] = ( 1.0 - ca ) * v1 * v2 - sa * v3;
  c[0][2] = ( 1.0 - ca ) * v1 * v3 + sa * v2;

  c[1][0] = ( 1.0 - ca ) * v2 * v1 + sa * v3;
  c[1][1] =                v2 * v2 + ca * ( 1.0 - v2 * v2 );
  c[1][2] = ( 1.0 - ca ) * v2 * v3 - sa * v1;

  c[2][0] = ( 1.0 - ca ) * v3 * v1 - sa * v2;
  c[2][1] = ( 1.0 - ca ) * v3 * v2 + sa * v1;
  c[2][2] =                v3 * v3 + ca * ( 1.0 - v3 * v3 );

  tmat_mxm ( c, a, d );

  for ( i = 0; i < 4; i++ )
  {
    for ( j = 0; j < 4; j++ )
    {
      b[i][j] = d[i][j];
    }
  }
  return;
}
//****************************************************************************80

void tmat_scale ( float a[4][4], float b[4][4], float sx, float sy,
  float sz )

//****************************************************************************80
//
//  Purpose:
//
//    TMAT_SCALE applies a scaling to the geometric transformation matrix.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Foley, van Dam, Feiner, Hughes,
//    Computer Graphics, Principles and Practice,
//    Addison Wesley, Second Edition, 1990.
//
//  Parameters:
//
//    Input, float A[4][4], the current geometric transformation matrix.
//
//    Output, float B[4][4], the modified geometric transformation matrix.
//    A and B may share the same memory.
//
//    Input, float SX, SY, SZ, the scalings to be applied to the X, Y and
//    Z coordinates.
//
{
  float c[4][4];
  float d[4][4];
  int i;
  int j;

  tmat_init ( c );

  c[0][0] = sx;
  c[1][1] = sy;
  c[2][2] = sz;

  tmat_mxm ( c, a, d );

  for ( i = 0; i < 4; i++ )
  {
    for ( j = 0; j < 4; j++ )
    {
      b[i][j] = d[i][j];
    }
  }
  return;
}
//****************************************************************************80

void tmat_shear ( float a[4][4], float b[4][4], char *axis, float s )

//****************************************************************************80
//
//  Purpose:
//
//    TMAT_SHEAR applies a shear to the geometric transformation matrix.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Foley, van Dam, Feiner, Hughes,
//    Computer Graphics, Principles and Practice,
//    Addison Wesley, Second Edition, 1990.
//
//  Parameters:
//
//    Input, float A[4][4], the current geometric transformation matrix.
//
//    Output, float B[4][4], the modified geometric transformation matrix.
//    A and B may share the same memory.
//
//    Input, character*3 AXIS, is 'XY', 'XZ', 'YX', 'YZ', 'ZX' or 'ZY',
//    specifying the shear equation:
//
//      XY:  x' = x + s * y;
//      XZ:  x' = x + s * z;
//      YX:  y' = y + s * x;
//      YZ:  y' = y + s * z;
//      ZX:  z' = z + s * x;
//      ZY:  z' = z + s * y.
//
//    Input, float S, the shear coefficient.
//
{
  float c[4][4];
  float d[4][4];
  int i;
  int j;

  tmat_init ( c );

  if ( strcmp ( axis, "XY" ) == 0 || strcmp ( axis, "xy" ) == 0 )
  {
    c[0][1] = s;
  }
  else if ( strcmp ( axis, "XZ" ) == 0 || strcmp ( axis, "xz" ) == 0 )
  {
    c[0][2] = s;
  }
  else if ( strcmp ( axis, "YX" ) == 0 || strcmp ( axis, "yx" ) == 0 )
  {
    c[1][0] = s;
  }
  else if ( strcmp ( axis, "YZ" ) == 0 || strcmp ( axis, "yz" ) == 0 )
  {
    c[1][2] = s;
  }
  else if ( strcmp ( axis, "ZX" ) == 0 || strcmp ( axis, "zx" ) == 0 )
  {
    c[2][0] = s;
  }
  else if ( strcmp ( axis, "ZY" ) == 0 || strcmp ( axis, "zy" ) == 0 )
  {
    c[2][1] = s;
  }
  else
  {
    cout << "\n";
    cout << "TMAT_SHEAR - Fatal error!\n";
    cout << "  Illegal shear axis: " << axis << ".\n";
    cout << "  Legal choices are XY, XZ, YX, YZ, ZX, or ZY.\n";
    return;
  }

  tmat_mxm ( c, a, d );

  for ( i = 0; i < 4; i++ )
  {
    for ( j = 0; j < 4; j++ )
    {
      b[i][j] = d[i][j];
    }
  }
  return;
}
//****************************************************************************80

void tmat_trans ( float a[4][4], float b[4][4], float x, float y,
  float z )

//****************************************************************************80
//
//  Purpose:
//
//    TMAT_TRANS applies a translation to the geometric transformation matrix.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 October 1998
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Foley, van Dam, Feiner, Hughes,
//    Computer Graphics, Principles and Practice,
//    Addison Wesley, Second Edition, 1990.
//
//  Parameters:
//
//    Input, float A[4][4], the current geometric transformation matrix.
//
//    Output, float B[4][4], the modified transformation matrix.
//    A and B may share the same memory.
//
//    Input, float X, Y, Z, the translation.  This may be thought of as the
//    point that the origin moves to under the translation.
//
{
  int i;
  int j;

  for ( i = 0; i < 4; i++ )
  {
    for ( j = 0; j < 4; j++ )
    {
      b[i][j] = a[i][j];
    }
  }
  b[0][3] = b[0][3] + x;
  b[1][3] = b[1][3] + y;
  b[2][3] = b[2][3] + z;

  return;
}
//****************************************************************************80

int tria_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    TRIA_READ reads an ASCII triangle file.
//
//  Example:
//
//    12                    <-- Number of triangles
//
//                          (x,y,z) and (nx,ny,nz) of normal vector at:
//
//    0.0 0.0 0.0 0.3 0.3 0.3   node 1 of triangle 1.
//    1.0 0.0 0.0 0.3 0.1 0.3   node 2 of triangle 1,
//    0.0 1.0 0.0 0.3 0.1 0.3   node 3 of triangle 1,
//    1.0 0.5 0.0 0.3 0.1 0.3   node 1 of triangle 2,
//    ...
//    0.0 0.5 0.5 0.3 0.1 0.3   node 3 of triangle 12.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    22 June 1999
//
//  Author:
//
//    John Burkardt
//
{
  float cvec[3];
  int icor3;
  int iface;
  int iface_hi;
  int iface_lo;
  int ivert;
  int face_num2;
  float r1;
  float r2;
  float r3;
  float r4;
  float r5;
  float r6;
//
//  Get the number of triangles.
//
  fgets ( input, LINE_MAX_LEN, filein );
  text_num = text_num + 1;
  sscanf ( input, "%d", &face_num2 );
//
//  For each triangle:
//
  iface_lo = face_num;
  iface_hi = face_num + face_num2;

  for ( iface = iface_lo; iface < iface_hi; iface++ )
  {
    if ( iface < FACE_MAX )
    {
      face_order[iface] = 3;
      face_material[iface] = 0;
    }
//
//  For each face:
//
    for ( ivert = 0; ivert < face_order[iface]; ivert++ )
    {
      fgets ( input, LINE_MAX_LEN, filein );
      text_num = text_num + 1;
      sscanf ( input, "%e %e %e %e %e %e", &r1, &r2, &r3, &r4, &r5, &r6 );

      cvec[0] = r1;
      cvec[1] = r2;
      cvec[2] = r3;

      if ( cor3_num < 1000 )
      {
        icor3 = rcol_find ( cor3, 3, cor3_num, cvec );
      }
      else
      {
        icor3 = -1;
      }

      if ( icor3 == -1 )
      {
        icor3 = cor3_num;
        if ( cor3_num < COR3_MAX )
        {
          cor3[0][cor3_num] = cvec[0];
          cor3[1][cor3_num] = cvec[1];
          cor3[2][cor3_num] = cvec[2];
        }
        cor3_num = cor3_num + 1;
      }
      else
      {
        dup_num = dup_num + 1;
      }

      if ( iface < FACE_MAX )
      {
        face[ivert][iface] = icor3;
        vertex_material[ivert][iface] = 0;
        vertex_normal[0][ivert][iface] = r4;
        vertex_normal[1][ivert][iface] = r5;
        vertex_normal[2][ivert][iface] = r6;
      }

    }
  }
  face_num = face_num + face_num2;

  return 0;
}
//****************************************************************************80

int tria_write ( FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    TRIA_WRITE writes the graphics data to an ASCII "triangle" file.
//
//  Discussion:
//
//    This is just a private format that Greg Hood requested from me.
//
//  Example:
//
//    12                    <-- Number of triangles
//
//                          (x,y,z) and (nx,ny,nz) of normal vector at:
//
//    0.0 0.0 0.0 0.3 0.3 0.3   node 1 of triangle 1.
//    1.0 0.0 0.0 0.3 0.1 0.3   node 2 of triangle 1,
//    0.0 1.0 0.0 0.3 0.1 0.3   node 3 of triangle 1,
//    1.0 0.5 0.0 0.3 0.1 0.3   node 1 of triangle 2,
//    ...
//    0.0 0.5 0.5 0.3 0.1 0.3   node 3 of triangle 12.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    10 June 1999
//
//  Author:
//
//    John Burkardt
//
{
  int face2[3];
  int icor3;
  int iface;
  int jlo;
  int k;
  int face_num2;
  int text_num;
  float nx;
  float ny;
  float nz;
  float x;
  float y;
  float z;

  text_num = 0;
//
//  Determine the number of triangular faces.
//
  face_num2 = 0;
  for ( iface = 0; iface < face_num; iface++ )
  {
    for ( jlo = 0; jlo < face_order[iface] - 2; jlo++ )
    {
      face_num2 = face_num2 + 1;
    }
  }

  fprintf ( fileout,  "%d\n", face_num2 );
  text_num = text_num + 1;
//
//  Do the next face.
//
  for ( iface = 0; iface < face_num; iface++ )
  {
//
//  Break the face up into triangles, anchored at node 1.
//
    for ( jlo = 0; jlo < face_order[iface] - 2; jlo++ )
    {
      face2[0] = face[    0][iface];
      face2[1] = face[jlo+1][iface];
      face2[2] = face[jlo+2][iface];

      for ( k = 0; k < 3; k++ )
      {
        icor3 = face2[k];

        x = cor3[0][icor3];
        y = cor3[1][icor3];
        z = cor3[2][icor3];

        nx = cor3_normal[0][icor3];
        ny = cor3_normal[1][icor3];
        nz = cor3_normal[2][icor3];

        fprintf ( fileout,  "%f %f %f %f %f %f\n", x, y, z, nx, ny, nz );

        text_num = text_num + 1;

      }

    }

  }
//
//  Report.
//
  cout << "\n";
  cout << "TRIA_WRITE - Wrote " << text_num << " text lines.\n";

  return 0;
}
//****************************************************************************80

int trib_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    TRIB_READ reads a binary triangle file.
//
//  Example:
//
//    4 byte int = number of triangles
//
//    For each triangular face:
//
//      3 4-byte floats = coordinates of first node;
//      3 4-byte floats = components of normal vector at first node;
//      3 4-byte floats = coordinates of second node;
//      3 4-byte floats = components of normal vector at second node;
//      3 4-byte floats = coordinates of third node;
//      3 4-byte floats = components of normal vector at third node.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    22 June 1999
//
//  Author:
//
//    John Burkardt
//
{
  float cvec[3];
  int icor3;
  int i;
  int iface;
  int iface_hi;
  int iface_lo;
  int ivert;
  int face_num2;
//
//  Read the number of triangles in the file.
//
  face_num2 = long_int_read ( filein );
  bytes_num = bytes_num + 4;
//
//  For each (triangular) face,
//    read the coordinates and normal vectors of three vertices,
//
  iface_lo = face_num;
  iface_hi = face_num + face_num2;

  for ( iface = iface_lo; iface < iface_hi; iface++ )
  {
    if ( iface < FACE_MAX )
    {
      face_order[iface] = 3;
      face_material[iface] = 0;
    }

    for ( ivert = 0; ivert < face_order[iface]; ivert++ )
    {
      for ( i = 0; i < 3; i++ )
      {
        cvec[i] = float_read ( filein );
        bytes_num = bytes_num + 4;
      }

      if ( cor3_num < 1000 )
      {
        icor3 = rcol_find ( cor3, 3, cor3_num, cvec );
      }
      else
      {
        icor3 = -1;
      }

      if ( icor3 == -1 )
      {
        icor3 = cor3_num;
        if ( cor3_num < COR3_MAX )
        {
          cor3[0][cor3_num] = cvec[0];
          cor3[1][cor3_num] = cvec[1];
          cor3[2][cor3_num] = cvec[2];
        }
        cor3_num = cor3_num + 1;
      }
      else
      {
        dup_num = dup_num + 1;
      }

      if ( iface < FACE_MAX )
      {
        face[ivert][iface] = icor3;
        vertex_material[ivert][iface] = 0;

        for ( i = 0; i < 3; i++ )
        {
          vertex_normal[i][ivert][iface] = float_read ( filein );
          bytes_num = bytes_num + 4;
        }

      }

    }
  }

  face_num = face_num + face_num2;

  return 0;
}
//****************************************************************************80

int trib_write ( FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    TRIB_WRITE writes the graphics data to a binary "triangle" file.
//
//  Discussion:
//
//    This is just a private format that Greg Hood requested from me.
//
//  Example:
//
//    12   Number of triangles
//    0.0  x at node 1, triangle 1,
//    0.0  y at node 1, triangle 1,
//    0.0  z at node 1, triangle 1,
//    0.3  nx at node 1, triangle 1,
//    0.3  ny at node 1, triangle 1,
//    0.3  nz at node 1, triangle 1.
//    1.0  x at node 2, triangle 1,
//    ...
//    0.7  nz at node 3, triangle 1.
//    1.2  x at node 1, triangle 2,
//    ...
//    0.3  nz at node 3, triangle 2.
//    9.3  x at node 1, triangle 3,
//    ...
//    0.3  nz at node 3, triangle 12.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    16 June 1999
//
//  Author:
//
//    John Burkardt
//
{
  int face2[3];
  int icor3;
  int iface;
  int jlo;
  int k;
  int face_num2;
  float nx;
  float ny;
  float nz;
  float x;
  float y;
  float z;

  bytes_num = 0;
//
//  Determine the number of triangular faces.
//
  face_num2 = 0;
  for ( iface = 0; iface < face_num; iface++ )
  {
    for ( jlo = 0; jlo < face_order[iface] - 2; jlo++ )
    {
      face_num2 = face_num2 + 1;
    }
  }

  bytes_num = bytes_num + long_int_write ( fileout, face_num2 );
//
//  Do the next face.
//
  for ( iface = 0; iface < face_num; iface++ )
  {
//
//  Break the face up into triangles, anchored at node 1.
//
    for ( jlo = 0; jlo < face_order[iface] - 2; jlo ++ )
    {
      face2[0] = face[    0][iface];
      face2[1] = face[jlo+1][iface];
      face2[2] = face[jlo+2][iface];

      for ( k = 0; k < 3; k++ )
      {
        icor3 = face2[k];

        x = cor3[0][icor3];
        y = cor3[1][icor3];
        z = cor3[2][icor3];

        nx = cor3_normal[0][icor3];
        ny = cor3_normal[1][icor3];
        nz = cor3_normal[2][icor3];

        bytes_num = bytes_num + float_write ( fileout, x );
        bytes_num = bytes_num + float_write ( fileout, y );
        bytes_num = bytes_num + float_write ( fileout, z );
        bytes_num = bytes_num + float_write ( fileout, nx );
        bytes_num = bytes_num + float_write ( fileout, ny );
        bytes_num = bytes_num + float_write ( fileout, nz );

      }

    }

  }
//
//  Report.
//
  cout << "\n";
  cout << "TRIB_WRITE - Wrote " << bytes_num << " bytes.\n";

  return 0;
}
//****************************************************************************80

int txt_write ( FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    TXT_WRITE writes the graphics data to a text file.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    25 June 1998
//
//  Author:
//
//    John Burkardt
//
{
  int i;
  int iface;
  int iline;
  int imat;
  int ivert;
  int nitem;
  int text_num;

  text_num = 0;

  fprintf ( fileout, "%s created by IVCON.\n", fileout_name );
  fprintf ( fileout, "Original data in %s.\n", filein_name );
  fprintf ( fileout, "Object name is %s.\n", object_name );
  fprintf ( fileout, "Object origin at %f %f %f.\n", origin[0], origin[1],
    origin[2] );
  fprintf ( fileout, "Object pivot at %f %f %f.\n", pivot[0], pivot[1],
    pivot[2] );
  text_num = text_num + 5;
//
//  TRANSFORMATION MATRIX.
//
  fprintf ( fileout, "\n" );
  fprintf ( fileout, "Transformation matrix:\n" );
  fprintf ( fileout, "\n" );
  text_num = text_num + 3;

  for ( i = 0; i < 4; i++ )
  {
    fprintf ( fileout, "  %f %f %f %f\n", transform_matrix[i][0],
      transform_matrix[i][1], transform_matrix[i][2], transform_matrix[i][3] );
      text_num = text_num + 1;
  }
//
//  NODES.
//
  fprintf ( fileout, "\n" );
  fprintf ( fileout, "  %d nodes.\n", cor3_num );
  text_num = text_num + 2;

  if ( 0 < cor3_num )
  {
    fprintf ( fileout, "\n" );
    fprintf ( fileout, "  Node coordinate data:\n" );
    fprintf ( fileout, "\n" );
    text_num = text_num + 3;

    for ( i = 0; i < cor3_num; i++ )
    {
      fprintf ( fileout, " %d %f %f %f\n ", i, cor3[0][i], cor3[1][i],
        cor3[2][i] );
      text_num = text_num + 1;
    }

    fprintf ( fileout, "\n" );
    fprintf ( fileout, "  Node normal vectors:\n" );
    fprintf ( fileout, "\n" );
    text_num = text_num + 3;

    for ( i = 0; i < cor3_num; i++ )
    {
      fprintf ( fileout, " %d %f %f %f\n ", i, cor3_normal[0][i],
        cor3_normal[1][i], cor3_normal[2][i] );
      text_num = text_num + 1;
    }

    fprintf ( fileout, "\n" );
    fprintf ( fileout, "  Node materials:\n" );
    fprintf ( fileout, "\n" );
    text_num = text_num + 3;

    for ( i = 0; i < cor3_num; i++ )
    {
      fprintf ( fileout, " %d %d\n ", i, cor3_material[i] );
      text_num = text_num + 1;
    }

    if ( 0 < texture_num )
    {
      fprintf ( fileout, "\n" );
      fprintf ( fileout, "  Node texture coordinates:\n" );
      fprintf ( fileout, "\n" );
      text_num = text_num + 3;

      for ( i = 0; i < cor3_num; i++ )
      {
        fprintf ( fileout, " %d %f %f\n ", i, cor3_tex_uv[0][i],
          cor3_tex_uv[1][i] );
        text_num = text_num + 1;
      }
    }
  }
//
//  LINES.
//
  fprintf ( fileout, "\n" );
  fprintf ( fileout, "  %d line data items.\n", line_num );
  text_num = text_num + 2;

  if ( 0 < line_num )
  {
    fprintf ( fileout, "\n" );
    fprintf ( fileout, "  Line index data:\n" );
    fprintf ( fileout, "\n" );
    text_num = text_num + 3;

    nitem = 0;

    for ( iline = 0; iline < line_num; iline++ )
    {
      fprintf ( fileout, " %d", line_dex[iline] );
      nitem = nitem + 1;

      if ( iline == line_num - 1 || line_dex[iline] == -1 || 10 <=  nitem )
      {
        nitem = 0;
        fprintf ( fileout, "\n" );
        text_num = text_num + 1;
      }

    }

    fprintf ( fileout, "\n" );
    fprintf ( fileout, "  Line materials:\n" );
    fprintf ( fileout, "\n" );
    text_num = text_num + 3;

    nitem = 0;

    for ( iline = 0; iline < line_num; iline++ )
    {

      fprintf ( fileout, " %d", line_material[iline] );
      nitem = nitem + 1;

      if ( iline == line_num - 1
        || line_material[iline] == -1 || 10 <= nitem )
      {
        nitem = 0;
        fprintf ( fileout, "\n" );
        text_num = text_num + 1;
      }
    }

  }
//
//  COLOR DATA
//
  fprintf ( fileout, "\n" );
  fprintf ( fileout, "  %d colors.\n", color_num );
  text_num = text_num + 2;
//
//  FACES.
//
  fprintf ( fileout, "\n" );
  fprintf ( fileout, "  %d faces.\n", face_num );
  text_num = text_num + 2;

  if ( 0 < face_num )
  {

    fprintf ( fileout, "\n" );
    fprintf ( fileout, "  Face, Material, Number of vertices, Smoothing, Flags:\n" );
    fprintf ( fileout, "\n" );
    text_num = text_num + 3;

    for ( iface = 0; iface < face_num; iface++ )
    {
      fprintf ( fileout, " %d %d %d %d %d\n", iface, face_material[iface],
        face_order[iface], face_smooth[iface], face_flags[iface] );
      text_num = text_num + 1;
    }

    fprintf ( fileout, "\n" );
    fprintf ( fileout, "  Face, Vertices\n" );
    fprintf ( fileout, "\n" );
    text_num = text_num + 3;

    for ( iface = 0; iface < face_num; iface++ )
    {
      fprintf ( fileout, "%d   ", iface );
      for ( ivert = 0; ivert < face_order[iface]; ivert++ )
      {
        fprintf ( fileout, " %d", face[ivert][iface] );
      }

      fprintf ( fileout, "\n" );
      text_num = text_num + 1;
    }

    fprintf ( fileout, "\n" );
    fprintf ( fileout, "  Face normal vectors:\n" );
    fprintf ( fileout, "\n" );
    text_num = text_num + 3;

    for ( iface = 0; iface < face_num; iface++ )
    {
      fprintf ( fileout, " %d %f %f %f\n", iface, face_normal[0][iface],
        face_normal[1][iface], face_normal[2][iface] );
      text_num = text_num + 1;
    }

    if ( 0 < texture_num )
    {
      fprintf ( fileout, "\n" );
      fprintf ( fileout, "  Face texture coordinates:\n" );
      fprintf ( fileout, "\n" );
      text_num = text_num + 3;

      for ( iface = 0; iface < face_num; iface++ )
      {
        fprintf ( fileout, " %d %f %f\n", iface, face_tex_uv[0][iface],
          face_tex_uv[1][iface] );
        text_num = text_num + 1;
      }
    }
  }
//
//  VERTICES.
//
  if ( 0 < face_num )
  {
    fprintf ( fileout, "\n" );
    fprintf ( fileout, "Vertex normal vectors:\n" );
    text_num = text_num + 2;

    for ( iface = 0; iface < face_num; iface++ )
    {
      fprintf ( fileout, "\n" );
      text_num = text_num + 1;
      for ( ivert = 0; ivert < face_order[iface]; ivert++ )
      {
        fprintf ( fileout, " %d %d %f %f %f\n", iface, ivert,
          vertex_normal[0][ivert][iface], vertex_normal[1][ivert][iface],
          vertex_normal[2][ivert][iface] );
        text_num = text_num + 1;
      }
    }

    fprintf ( fileout, "\n" );
    fprintf ( fileout, "Vertex materials:\n" );
    fprintf ( fileout, "\n" );
    text_num = text_num + 3;

    for ( iface = 0; iface < face_num; iface++ )
    {
      fprintf ( fileout, "%d", iface );
      for ( ivert = 0; ivert < face_order[iface]; ivert++ )
      {
        fprintf ( fileout, " %d", vertex_material[ivert][iface] );
      }
      fprintf ( fileout, "\n" );
      text_num = text_num + 1;
    }

    if ( 0 < texture_num )
    {
      fprintf ( fileout, "\n" );
      fprintf ( fileout, "Vertex UV texture coordinates:\n" );
      fprintf ( fileout, "\n" );
      text_num = text_num + 3;

      for ( iface = 0; iface < face_num; iface++ )
      {
        for ( ivert = 0; ivert < face_order[iface]; ivert++ )
        {
          fprintf ( fileout, "%d %d %f %f\n", iface, ivert,
            vertex_tex_uv[0][ivert][iface], vertex_tex_uv[1][ivert][iface] );
          text_num = text_num + 1;
        }
      }
    }
  }
//
//  MATERIALS.
//
  fprintf ( fileout, "\n" );
  fprintf ( fileout, "%d materials.\n", material_num );
  fprintf ( fileout, "\n" );
  fprintf ( fileout, "Index      Name   R G B A\n" );
  fprintf ( fileout, "\n" );

  text_num = text_num + 5;

  for ( imat = 0; imat < material_num; imat++ )
  {
    fprintf ( fileout, "%d %s %f %f %f %f\n", imat, material_name[imat],
      material_rgba[0][imat], material_rgba[1][imat], material_rgba[2][imat],
      material_rgba[3][imat] );
    text_num = text_num + 1;
  }
//
//  TEXTURES.
//
  fprintf ( fileout, "\n" );
  fprintf ( fileout, "%d textures.\n", texture_num );
  text_num = text_num + 2;

  if ( 0 < texture_num )
  {
    fprintf ( fileout, "\n" );
    fprintf ( fileout, "Index  Name\n" );
    fprintf ( fileout, "\n" );
    for ( i = 0; i < texture_num; i++ )
    {
      fprintf ( fileout, "%d %s\n", i, texture_name[i] );
    }
    text_num = text_num + 3;
  }
//
//  Report.
//
  cout << "\n";
  cout << "TXT_WRITE - Wrote " << text_num << " text lines.\n";

  return 0;
}
//****************************************************************************80

int ucd_write ( FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    UCD_WRITE writes graphics data to an AVS UCD file.
//
//  Example:
//
//    #  cube.ucd created by IVREAD.
//    #
//    #  Material RGB to hue map:
//    #
//    #  material    R    G  B   Alpha     Hue
//    #
//    #    0   0.94  0.70  0.15  1.000   0.116
//    #    1   0.24  0.70  0.85  1.000   0.541
//    #    2   0.24  0.00  0.85  1.000   0.666
//    #
//    #  The node data is
//    #    node # / material # / RGBA / Hue
//    #
//    8  6  6  0  0
//    0  0.0  0.0  0.0
//    1  1.0  0.0  0.0
//    2  1.0  1.0  0.0
//    3  0.0  1.0  0.0
//    4  0.0  0.0  1.0
//    5  1.0  0.0  1.0
//    6  1.0  1.0  1.0
//    7  0.0  1.0  1.0
//    0  0  quad  0  1  2  3
//    1  0  quad  0  4  5  1
//    2  0  quad  1  5  6  2
//    3  0  quad  2  6  7  3
//    4  0  quad  3  7  4  0
//    5  0  quad  4  7  6  5
//    3  1 4 1
//    material, 0...2
//    RGBA, 0-1/0-1/0-1/0-1
//    Hue, 0-1
//    0  0  0.94  0.70  0.15  1.0  0.116
//    1  0  0.94  0.70  0.15  1.0  0.116
//    2  0  0.94  0.70  0.15  1.0  0.116
//    3  0  0.94  0.70  0.15  1.0  0.116
//    4  1  0.24  0.70  0.85  1.0  0.541
//    5  1  0.24  0.70  0.85  1.0  0.541
//    6  2  0.24  0.24  0.85  0.0  0.666
//    7  2  0.24  0.24  0.85  0.0  0.666
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    22 May 1999
//
//  Author:
//
//    John Burkardt
//
//
{
  float a;
  float b;
  float g;
  float h;
  int i;
  int imat;
  int j;
  int text_num;
  float r;

  text_num = 0;

  fprintf ( fileout, "#  %s created by IVREAD.\n", fileout_name );
  fprintf ( fileout, "#\n" );
  fprintf ( fileout, "#  Material RGB to Hue map:\n" );
  fprintf ( fileout, "#\n" );
  fprintf ( fileout, "#  material    R    G      B     Alpha  Hue\n" );
  fprintf ( fileout, "#\n" );

  text_num = text_num + 6;

  for ( j = 0; j < material_num; j++ )
  {
    r = material_rgba[0][j];
    g = material_rgba[1][j];
    b = material_rgba[2][j];
    a = material_rgba[3][j];
    h = rgb_to_hue ( r, g, b );
    fprintf ( fileout, "#  %d %f %f %f %f %f\n", j, r, g, b, a, h );
    text_num = text_num + 1;
  }

  fprintf ( fileout, "#\n" );
  fprintf ( fileout, "#  The node data is\n" );
  fprintf ( fileout, "#    node # / material # / RGBA / Hue\n" );
  fprintf ( fileout, "#\n" );
  text_num = text_num + 4;

  fprintf ( fileout, "%d %d 6 0 0\n", cor3_num, face_num );
  text_num = text_num + 1;

  for ( j = 0; j < cor3_num; j++ )
  {
    fprintf ( fileout, "%d %f %f %f\n", j, cor3[0][j], cor3[1][j],
      cor3[2][j] );
    text_num = text_num + 1;
  }
//
//  NOTE:
//  UCD only accepts triangles and quadrilaterals, not higher order
//  polygons.  We would need to break polygons up to proceed.
//
  for ( j = 0; j < face_num; j++ )
  {
    fprintf ( fileout, "%d %d", j, face_material[j] );

    if ( face_order[j] == 3 )
    {
      fprintf ( fileout, " tri" );
    }
    else if ( face_order[j] == 4 )
    {
      fprintf ( fileout, " quad" );
    }
    else
    {
      fprintf ( fileout, " ???" );
    }

    for ( i = 0; i < face_order[j]; i++ )
    {
      fprintf ( fileout, "%d", face[i][j] );
    }
    fprintf ( fileout, "\n" );
    text_num = text_num + 1;

  }

  fprintf ( fileout, "3  1  4  1\n" );
  fprintf ( fileout, "material, 0...%d\n", material_num - 1 );
  fprintf ( fileout, "RGBA, 0-1/0-1/0-1/0-1\n" );
  fprintf ( fileout, "Hue, 0-1\n" );
  text_num = text_num + 4;

  for ( j = 0; j < cor3_num; j++ )
  {
    imat = cor3_material[j];
    r = material_rgba[0][imat];
    g = material_rgba[1][imat];
    b = material_rgba[2][imat];
    a = material_rgba[3][imat];
    h = rgb_to_hue ( r, g, b );

    fprintf ( fileout, "%d %d %f %f %f %f %f\n", j, imat, r, g, b, a, h );
    text_num = text_num + 1;
  }
//
//  Report.
//
  cout << "\n";
  cout << "UCD_WRITE - Wrote " << text_num << " text lines.\n";

  return 0;
}
//****************************************************************************80

void vertex_normal_set ( )

//****************************************************************************80
//
//  Purpose:
//
//    VERTEX_NORMAL_SET recomputes the face vertex normal vectors.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    12 October 1998
//
//  Author:
//
//    John Burkardt
//
{
  int i;
  int i0;
  int i1;
  int i2;
  int iface;
  int ivert;
  int jp1;
  int jp2;
  int nfix;
  float norm;
  float temp;
  float x0;
  float x1;
  float x2;
  float xc;
  float y0;
  float y1;
  float y2;
  float yc;
  float z0;
  float z1;
  float z2;
  float zc;

  if ( face_num <= 0 )
  {
    return;
  }

  nfix = 0;
//
//  Consider each face.
//
  for ( iface = 0; iface < face_num; iface++ )
  {
    for ( ivert = 0; ivert < face_order[iface]; ivert++ )
    {
      norm = 0.0;
      for ( i = 0; i < 3; i++ )
      {
        temp = vertex_normal[i][ivert][iface];
        norm = norm + temp * temp;
      }
      norm = ( float ) sqrt ( norm );

      if ( norm == 0.0 )
      {
        nfix = nfix + 1;

        i0 = face[ivert][iface];
        x0 = cor3[0][i0];
        y0 = cor3[1][i0];
        z0 = cor3[2][i0];

        jp1 = ivert + 1;
        if ( face_order[iface] <= jp1 )
        {
          jp1 = jp1 - face_order[iface];
        }
        i1 = face[jp1][iface];
        x1 = cor3[0][i1];
        y1 = cor3[1][i1];
        z1 = cor3[2][i1];

        jp2 = ivert + 2;
        if ( face_order[iface] <= jp2 )
        {
          jp2 = jp2 - face_order[iface];
        }
        i2 = face[jp2][iface];
        x2 = cor3[0][i2];
        y2 = cor3[1][i2];
        z2 = cor3[2][i2];

        xc = ( y1 - y0 ) * ( z2 - z0 ) - ( z1 - z0 ) * ( y2 - y0 );
        yc = ( z1 - z0 ) * ( x2 - x0 ) - ( x1 - x0 ) * ( z2 - z0 );
        zc = ( x1 - x0 ) * ( y2 - y0 ) - ( y1 - y0 ) * ( x2 - x0 );

        norm = ( float ) sqrt ( xc * xc + yc * yc + zc * zc );

        if ( norm == 0.0 )
        {
          xc = ( float ) 1.0 / sqrt ( 3.0 );
          yc = ( float ) 1.0 / sqrt ( 3.0 );
          zc = ( float ) 1.0 / sqrt ( 3.0 );
        }
        else
        {
          xc = xc / norm;
          yc = yc / norm;
          zc = zc / norm;
        }

        vertex_normal[0][ivert][iface] = xc;
        vertex_normal[1][ivert][iface] = yc;
        vertex_normal[2][ivert][iface] = zc;

      }
    }
  }

  if ( 0 < nfix )
  {
    cout << "\n";
    cout << "VERTEX_NORMAL_SET: Recomputed " << nfix
         << " face vertex normals.\n";
  }

  return;
}
//****************************************************************************80

void vertex_to_face_material ( )

//****************************************************************************80
//
//  Purpose:
//
//    VERTEX_TO_FACE_MATERIAL extends vertex material definitions to faces.
//
//  Discussion:
//
//    Assuming material indices are defined for all the vertices, this
//    routine assigns to each face the material associated with its
//    first vertex.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    22 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  int iface;
  int ivert;

  ivert = 0;
  for ( iface = 0; iface < face_num; iface++ )
  {
    face_material[iface] = vertex_material[ivert][iface];
  }

  return;
}
//****************************************************************************80

void vertex_to_node_material ( )

//****************************************************************************80
//
//  Purpose:
//
//    VERTEX_TO_NODE_MATERIAL extends vertex material definitions to nodes.
//
//  Discussion:
//
//    A NODE is a point in space.
//    A VERTEX is a node as used in a particular face.
//    One node may be used as a vertex in several faces, or none.
//    This routine simply runs through all the vertices, and assigns
//    the material of the vertex to the corresponding node.  If a
//    node appears as a vertex several times, then the node will
//    end up having the material of the vertex that occurs "last".
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    22 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  int iface;
  int ivert;
  int node;

  for ( iface = 0; iface < face_num; iface++ )
  {
    for ( ivert = 0; ivert < face_order[iface]; ivert++ )
    {
      node = face[ivert][iface];
      cor3_material[node] = vertex_material[ivert][iface];
    }
  }

  return;
}

//****************************************************************************80

int vla_read ( FILE *filein )

//****************************************************************************80
//
//  Purpose:
//
//    VLA_READ reads a VLA file.
//
//  Example:
//
//     set comment cube.vla created by IVREAD
//     set comment Original data in cube.iv.
//     set comment
//     set intensity EXPLICIT
//     set parametric NON_PARAMETRIC
//     set filecontent LINES
//     set filetype NEW
//     set depthcue 0
//     set defaultdraw stellar
//     set coordsys RIGHT
//     set author IVREAD
//     set site Buhl Planetarium
//     set library_id UNKNOWN
//     P   8.59816       5.55317      -3.05561       1.00000
//     L   8.59816       2.49756      0.000000E+00   1.00000
//     L   8.59816       2.49756      -3.05561       1.00000
//     L   8.59816       5.55317      -3.05561       1.00000
//     P   8.59816       5.55317      0.000000E+00   1.00000
//     ...etc...
//     L   2.48695       2.49756      -3.05561       1.00000
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    23 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  int i;
  int   icor3;
  int   dup_num;
  char *next;
  int   text_num;
  float r1;
  float r2;
  float r3;
  float temp[3];
  char  token[LINE_MAX_LEN];
  int   width;
//
//  Initialize.
//
  dup_num = 0;
  text_num = 0;
//
//  Read the next line of the file into INPUT.
//
  while ( fgets ( input, LINE_MAX_LEN, filein ) != NULL )
  {
    text_num = text_num + 1;
//
//  Advance to the first nonspace character in INPUT.
//
    for ( next = input; *next != '\0' && ch_is_space ( *next ); next++ )
    {
    }
//
//  Skip blank lines and comments.
//
    if ( *next == '\0' || *next == ';' )
    {
      continue;
    }
//
//  Extract the first word in this line.
//
    sscanf ( next, "%s%n", token, &width );
//
//  Set NEXT to point to just after this token.
//
    next = next + width;
//
//  SET (ignore)
//
    if ( s_eqi ( token, "set" ) )
    {
    }
//
//  P (begin a line)
//  L (continue a line)
//
    else if ( s_eqi ( token, "P" ) || s_eqi ( token, "L" ) )
    {
      if ( s_eqi ( token, "P" ) )
      {
        if ( 0 < line_num )
        {
          if ( line_num < LINES_MAX )
          {
            line_dex[line_num] = -1;
            line_material[line_num] = -1;
            line_num = line_num + 1;
          }
        }
      }

      sscanf ( next, "%e %e %e", &r1, &r2, &r3 );

      temp[0] = r1;
      temp[1] = r2;
      temp[2] = r3;

      if ( cor3_num < 1000 )
      {
        icor3 = rcol_find ( cor3, 3, cor3_num, temp );
      }
      else
      {
        icor3 = -1;
      }

      if ( icor3 == -1 )
      {
        icor3 = cor3_num;

        if ( cor3_num < COR3_MAX )
        {
          for ( i = 0; i < 3; i++ )
          {
            cor3[i][cor3_num] = temp[i];
          }
        }
        cor3_num = cor3_num + 1;
      }
      else
      {
        dup_num = dup_num + 1;
      }

      if ( line_num < LINES_MAX )
      {
        line_dex[line_num] = icor3;
        line_material[line_num] = 0;
        line_num = line_num + 1;
      }
    }
//
//  Unexpected or unrecognized.
//
    else
    {
      cout << "\n";
      cout << "VLA_READ - Fatal error!\n";
      cout << "  Unrecognized first word on line.\n";
      return 1;
    }

  }

  if ( 0 < line_num )
  {
    if ( line_num < LINES_MAX )
    {
      line_dex[line_num] = -1;
      line_material[line_num] = -1;
      line_num = line_num + 1;
    }
  }

  return 0;
}
//****************************************************************************80

int vla_write ( FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    VLA_WRITE writes internal graphics information to a VLA file.
//
//  Discussion:
//
//    Comments begin with a semicolon in column 1.
//    The X, Y, Z coordinates of points begin with a "P" to
//    denote the beginning of a line, and "L" to denote the
//    continuation of a line.  The fourth entry is intensity, which
//    should be between 0.0 and 1.0.
//
//  Example:
//
//     set comment cube.vla created by IVREAD
//     set comment Original data in cube.iv.
//     set comment
//     set intensity EXPLICIT
//     set parametric NON_PARAMETRIC
//     set filecontent LINES
//     set filetype NEW
//     set depthcue 0
//     set defaultdraw stellar
//     set coordsys RIGHT
//     set author IVREAD
//     set site Buhl Planetarium
//     set library_id UNKNOWN
//     P   8.59816       5.55317      -3.05561       1.00000
//     L   8.59816       2.49756      0.000000E+00   1.00000
//     L   8.59816       2.49756      -3.05561       1.00000
//     L   8.59816       5.55317      -3.05561       1.00000
//     P   8.59816       5.55317      0.000000E+00   1.00000
//     ...etc...
//     L   2.48695       2.49756      -3.05561       1.00000
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    22 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  char  c;
  int   iline;
  float intense = 1.0;
  int   k;
  int   text_num;
//
//  Initialize.
//
  text_num = 0;

  fprintf ( fileout, "set comment %s created by IVCON.\n", fileout_name );
  fprintf ( fileout, "set comment Original data in %s.\n", filein_name );
  fprintf ( fileout, "set comment\n" );
  fprintf ( fileout, "set intensity EXPLICIT\n" );
  fprintf ( fileout, "set parametric NON_PARAMETRIC\n" );
  fprintf ( fileout, "set filecontent LINES\n" );
  fprintf ( fileout, "set filetype NEW\n" );
  fprintf ( fileout, "set depthcue 0\n" );
  fprintf ( fileout, "set defaultdraw stellar\n" );
  fprintf ( fileout, "set coordsys RIGHT\n" );
  fprintf ( fileout, "set author IVCON\n" );
  fprintf ( fileout, "set site Buhl Planetarium\n" );
  fprintf ( fileout, "set library_id UNKNOWN\n" );

  text_num = text_num + 13;

  c = 'P';

  for ( iline = 0; iline < line_num; iline++ )
  {

    k = line_dex[iline];

    if ( k == -1 )
    {
      c = 'P';
    }
    else
    {
      fprintf ( fileout, "%c %f %f %f %f\n",
        c, cor3[0][k], cor3[1][k], cor3[2][k], intense );

      text_num = text_num + 1;

      c = 'L';
    }
  }
//
//  Report.
//
  cout << "\n";
  cout << "VLA_WRITE - Wrote " << text_num << " text lines.\n";

  return 0;
}
//****************************************************************************80

int wrl_write ( FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    WRL_WRITE writes graphics data to a WRL file.
//
//  Example:
//
//    #VRML V2.0 utf8
//
//    WorldInfo {
//      title "cube.iv."
//      string "WRL file generated by IVREAD.
//    }
//
//    Group {
//      children [
//
//        Shape {
//
//          appearance Appearance {
//            material Material {
//              diffuseColor   0.0 0.0 0.0
//              emissiveColor  0.0 0.0 0.0
//              shininess      1.0
//            }
//          } #end of appearance
//
//          geometry IndexedLineSet {
//
//            coord Coordinate {
//              point [
//                8.59816       5.55317      -3.05561
//                8.59816       2.49756      0.000000E+00
//                ...etc...
//                2.48695       2.49756      -3.05561
//              ]
//            }
//
//            coordIndex [
//                0     1     2    -1     3     4     5     6     7     8    -
//                9    10    -1    11    12    -1    13    14    15    -1    1
//              ...etc...
//              191    -1
//            ]
//
//            colorPerVertex TRUE
//
//            colorIndex [
//                0     0     0    -1     2     3     1     1     4     7    -
//               10     9    -1     7     7    -1     3     2     2    -1    1
//              ...etc...
//              180    -1
//            ]
//
//          }  #end of geometry
//
//        }  #end of Shape
//
//      ]  #end of children
//
//    }  #end of Group
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    23 May 1999
//
//  Author:
//
//    John Burkardt
//
{
  int icor3;
  int iface;
  int itemp;
  int ivert;
  int j;
  int length;
  int ndx;

  text_num = 0;

  fprintf ( fileout, "#VRML V2.0 utf8\n" );
  fprintf ( fileout, "\n" );
  fprintf ( fileout, "  WorldInfo {\n" );
  fprintf ( fileout, "    title \"%s\"\n", fileout_name );
  fprintf ( fileout, "    info \"WRL file generated by IVREAD.\"\n" );
  fprintf ( fileout, "    info \"Original data in %s\"\n", filein_name );
  fprintf ( fileout, "  }\n" );
  fprintf ( fileout, "\n" );
  fprintf ( fileout, "  Group {\n" );
  fprintf ( fileout, "    children [\n" );
  fprintf ( fileout, "      Shape {\n" );
  fprintf ( fileout, "        appearance Appearance {\n" );
  fprintf ( fileout, "          material Material {\n" );
  fprintf ( fileout, "            diffuseColor   0.0 0.0 0.0\n" );
  fprintf ( fileout, "            emissiveColor  0.0 0.0 0.0\n" );
  fprintf ( fileout, "            shininess      1.0\n" );
  fprintf ( fileout, "          }\n" );
  fprintf ( fileout, "        }\n" );

  text_num = text_num + 18;
//
//  IndexedLineSet
//
  if ( 0 < line_num )
  {
    fprintf ( fileout, "        geometry IndexedLineSet {\n" );
//
//  IndexedLineSet coord
//
    fprintf ( fileout, "          coord Coordinate {\n" );
    fprintf ( fileout, "            point [\n" );

    text_num = text_num + 3;

    for ( icor3 = 0; icor3 < cor3_num; icor3++ )
    {
      fprintf ( fileout, "              %f %f %f\n", cor3[0][icor3],
        cor3[1][icor3], cor3[2][icor3] );
      text_num = text_num + 1;
    }

    fprintf ( fileout, "            ]\n" );
    fprintf ( fileout, "          }\n" );
    text_num = text_num + 2;
//
//  IndexedLineSet coordIndex.
//
    fprintf ( fileout, "          coordIndex [\n" );

    text_num = text_num + 1;

    length = 0;
    for ( j = 0; j < line_num; j++ )
    {
      fprintf ( fileout, "%d ", line_dex[j] );
      length = length + 1;
      if ( line_dex[j] == -1 || 10 <= length || j == line_num - 1 )
      {
        fprintf ( fileout, "\n" );
        text_num = text_num + 1;
        length = 0;
      }
    }

    fprintf ( fileout, "          ]\n" );
    text_num = text_num + 1;
//
//  Colors. (materials)
//
    fprintf ( fileout, "          color Color {\n" );
    fprintf ( fileout, "            color [\n" );
    text_num = text_num + 2;

    for ( j = 0; j < material_num; j++ )
    {
      fprintf ( fileout, "              %f %f %f\n", material_rgba[0][j],
        material_rgba[1][j], material_rgba[2][j] );
      text_num = text_num + 1;
    }

    fprintf ( fileout, "            ]\n" );
    fprintf ( fileout, "          }\n" );
    fprintf ( fileout, "          colorPerVertex TRUE\n" );
//
//  IndexedLineset colorIndex
//
    fprintf ( fileout, "          colorIndex [\n" );

    text_num = text_num + 4;

    length = 0;
    for ( j = 0; j < line_num; j++ )
    {
      fprintf ( fileout, "%d ", line_material[j] );
      length = length + 1;
      if ( line_dex[j] == -1 || 10 <= length || j == line_num - 1 )
      {
        fprintf ( fileout, "\n" );
        text_num = text_num + 1;
        length = 0;
      }
    }

    fprintf ( fileout, "          ]\n" );
    fprintf ( fileout, "        }\n" );
    text_num = text_num + 2;

  }
//
//  End of IndexedLineSet
//
//  IndexedFaceSet
//
  if ( 0 < face_num )
  {

    fprintf ( fileout, "        geometry IndexedFaceSet {\n" );
//
//  IndexedFaceSet coord
//
    fprintf ( fileout, "          coord Coordinate {\n" );
    fprintf ( fileout, "            point [\n" );

    text_num = text_num + 3;

    for ( icor3 = 0; icor3 < cor3_num; icor3++ )
    {
      fprintf ( fileout, "              %f %f %f\n", cor3[0][icor3],
        cor3[1][icor3], cor3[2][icor3] );

      text_num = text_num + 1;
    }

    fprintf ( fileout, "            ]\n" );
    fprintf ( fileout, "          }\n" );
//
//  IndexedFaceSet coordIndex.
//
    fprintf ( fileout, "          coordIndex [\n" );

    text_num = text_num + 3;

    length = 0;

    for ( iface = 0; iface < face_num; iface++ )
    {
      for ( ivert = 0; ivert <= face_order[iface]; ivert++ )
      {
        if ( ivert <= face_order[iface] )
        {
          itemp = face[ivert][iface];
        }
        else
        {
          itemp = 0;
        }

        fprintf ( fileout, "%d ", itemp );
        length = length + 1;

        if ( itemp == -1 || 10 <= length ||
          ( iface == face_num - 1 && ivert == face_order[iface]  ) )
        {
          fprintf ( fileout, "\n" );
          text_num = text_num + 1;
          length = 0;
        }

      }

    }

    fprintf ( fileout, "          ]\n" );
    text_num = text_num + 1;
//
//  IndexedFaceset colorIndex
//
    fprintf ( fileout, "          colorIndex [\n" );
    text_num = text_num + 1;

    length = 0;
    ndx = 0;

    for ( iface = 0; iface < face_num; iface++ )
    {
      for ( ivert = 0; ivert <= face_order[iface]; ivert++ )
      {

        if ( ivert <= face_order[iface] )
        {
          itemp = vertex_material[ivert][iface];
          ndx = ndx + 1;
        }
        else
        {
          itemp = 0;
        }

        fprintf ( fileout, "%d ", itemp );
        length = length + 1;

        if ( itemp == -1 || 10 <= length ||
          ( iface == face_num - 1 && ivert == face_order[iface] )  )
        {

          fprintf ( fileout, "\n" );
          text_num = text_num + 1;
          length = 0;

        }

      }

    }

    fprintf ( fileout, "          ]\n" );
    fprintf ( fileout, "        }\n" );
    text_num = text_num + 2;

  }
//
//  End of IndexedFaceSet
//
//  End of:
//    Shape
//  children
//    Group
//
  fprintf ( fileout, "      }\n" );
  fprintf ( fileout, "    ]\n" );
  fprintf ( fileout, "  }\n" );

  text_num = text_num + 3;
//
//  Report.
//
  cout << "\n";
  cout << "WRL_WRITE - Wrote " << text_num << " text lines.\n";

  return 0;
}
//****************************************************************************80

int xgl_write ( FILE *fileout )

//****************************************************************************80
//
//  Purpose:
//
//    XGL_WRITE writes an XGL file.
//
//  Discussion:
//
//    Two corrections to the routine were pointed out by
//    Mike Phillips, msphil AT widowmaker.com, on 17 September 2001,
//    and are gratefully acknowledged.
//
//  Example:
//
//    <WORLD>
//
//      <BACKGROUND>
//        <BACKCOLOR> 0.1, 0.1, 0.1 </BACKCOLOR>
//      </BACKGROUND>
//
//      <LIGHTING>
//        <AMBIENT> 0.2, 0.1, 0.1 </AMBIENT>
//        <DIRECTIONALLIGHT>
//          <DIFFUSE> 0.1, 0.2, 0.1 </DIFFUSE>
//          <DIRECTION> 0, 0, 100 </DIRECTION>
//          <SPECULAR> 0.1, 0.1, 0.2 </SPECULAR>
//        </DIRECTIONALLIGHT>
//      </LIGHTING>
//
//      <MESH ID = "0">
//
//        <P ID="0"> -0.5, -0.5, 1 </P>
//        <P ID="1"> 0.5, -0.5, 1 </P>
//        <P ID="2"> 0.5, 0.5, 1 </P>
//        <P ID="3"> -0.5, 0.5, 1 </P>
//        <P ID="4"> 0.5, -0.5, 0 </P>
//        <P ID="5"> -0.5, -0.5, 0 </P>
//        <P ID="6"> -0.5, 0.5, 0 </P>
//        <P ID="7"> 0.5, 0.5, 0 </P>
//
//        <N ID="0"> -0.408248, -0.408248, 0.816497 </N>
//        <N ID="1"> 0.666667, -0.666667, 0.333333 </N>
//        <N ID="2"> 0.408248, 0.408248, 0.816497 </N>
//        <N ID="3"> -0.666667, 0.666667, 0.333333 </N>
//        <N ID="4"> 0.408248, -0.408248, -0.816497 </N>
//        <N ID="5"> -0.666667, -0.666667, -0.333333 </N>
//        <N ID="6"> -0.408248, 0.408248, -0.816497 </N>
//        <N ID="7"> 0.666667, 0.666667, -0.333333 </N>
//
//        <MAT ID="0">
//          <ALPHA> 0.9 </ALPHA>
//          <AMB> 0.1, 0.1, 0.1 </AMB>
//          <DIFF> 0.2, 0.1, 0.1 </DIFF>
//          <EMISS> 0.1, 0.2, 0.1 </EMISS>
//          <SHINE> 0.8 </SHINE>
//          <SPEC> 0.1, 0.1, 0.2 </SPEC>
//        </MAT>
//
//        <F>
//          <MATREF> 0 </MATREF>
//          <FV1><PREF> 0 </PREF><NREF> 0 </NREF></FV1>
//          <FV2><PREF> 1 </PREF><NREF> 1 </NREF></FV2>
//          <FV3><PREF> 2 </PREF><NREF> 2 </NREF></FV3>
//        </F>
//        <F>
//          <MATREF> 0 </MATREF>
//          <FV1><PREF> 0 </PREF><NREF> 0 </NREF></FV1>
//          <FV2><PREF> 2 </PREF><NREF> 2 </NREF></FV2>
//          <FV3><PREF> 3 </PREF><NREF> 3 </NREF></FV3>
//        </F>
//        <F>
//          <MATREF> 0 </MATREF>
//          <FV1><PREF> 4 </PREF><NREF> 4 </NREF></FV1>
//          <FV2><PREF> 5 </PREF><NREF> 5 </NREF></FV2>
//          <FV3><PREF> 6 </PREF><NREF> 6 </NREF></FV3>
//        </F>
//        <F>
//          <MATREF> 0 </MATREF>
//          <FV1><PREF> 4 </PREF><NREF> 4 </NREF></FV1>
//          <FV2><PREF> 6 </PREF><NREF> 6 </NREF></FV2>
//          <FV3><PREF> 7 </PREF><NREF> 7 </NREF></FV3>
//        </F>
//        <F>
//          <MATREF> 0 </MATREF>
//          <FV1><PREF> 5 </PREF><NREF> 5 </NREF></FV1>
//          <FV2><PREF> 0 </PREF><NREF> 0 </NREF></FV2>
//          <FV3><PREF> 3 </PREF><NREF> 3 </NREF></FV3>
//        </F>
//        <F>
//          <MATREF> 0 </MATREF>
//          <FV1><PREF> 5 </PREF><NREF> 5 </NREF></FV1>
//          <FV2><PREF> 3 </PREF><NREF> 3 </NREF></FV2>
//          <FV3><PREF> 6 </PREF><NREF> 6 </NREF></FV3>
//        </F>
//        <F>
//          <MATREF> 0 </MATREF>
//          <FV1><PREF> 1 </PREF><NREF> 1 </NREF></FV1>
//          <FV2><PREF> 4 </PREF><NREF> 4 </NREF></FV2>
//          <FV3><PREF> 7 </PREF><NREF> 7 </NREF></FV3>
//        </F>
//        <F>
//          <MATREF> 0 </MATREF>
//          <FV1><PREF> 1 </PREF><NREF> 1 </NREF></FV1>
//          <FV2><PREF> 7 </PREF><NREF> 7 </NREF></FV2>
//          <FV3><PREF> 2 </PREF><NREF> 2 </NREF></FV3>
//        </F>
//        <F>
//          <MATREF> 0 </MATREF>
//          <FV1><PREF> 5 </PREF><NREF> 5 </NREF></FV1>
//          <FV2><PREF> 4 </PREF><NREF> 4 </NREF></FV2>
//          <FV3><PREF> 1 </PREF><NREF> 1 </NREF></FV3>
//        </F>
//        <F>
//          <MATREF> 0 </MATREF>
//          <FV1><PREF> 5 </PREF><NREF> 5 </NREF></FV1>
//          <FV2><PREF> 1 </PREF><NREF> 1 </NREF></FV2>
//          <FV3><PREF> 0 </PREF><NREF> 0 </NREF></FV3>
//        </F>
//        <F>
//          <MATREF> 0 </MATREF>
//          <FV1><PREF> 3 </PREF><NREF> 3 </NREF></FV1>
//          <FV2><PREF> 2 </PREF><NREF> 2 </NREF></FV2>
//          <FV3><PREF> 7 </PREF><NREF> 7 </NREF></FV3>
//        </F>
//        <F>
//          <MATREF> 0 </MATREF>
//          <FV1><PREF> 3 </PREF><NREF> 3 </NREF></FV1>
//          <FV2><PREF> 7 </PREF><NREF> 7 </NREF></FV2>
//          <FV3><PREF> 6 </PREF><NREF> 6 </NREF></FV3>
//        </F>
//      </MESH>
//
//      <OBJECT>
//        <TRANSFORM>
//          <FORWARD> 0, 0, 0 </FORWARD>
//          <POSITION> 0, 0, 0 </POSITION>
//          <SCALE> 1, 1, 1 </SCALE>
//          <UP> 1, 1, 1 </UP>
//        </TRANSFORM>
//        <MESHREF> 0 </MESHREF>
//      </OBJECT>
//
//    </WORLD>
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    17 September 2001
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    XGL specification at http://www.xglspec.org/
//
{
  int iface;
  int ivert;
  int j;
  float light_ambient_rgb[3];
  float light_diffuse_rgb[3];
  float light_direction[3];
  float light_specular_rgb[3];
  int material;
  float material_alpha;
  float material_amb_rgb[3];
  float material_diff_rgb[3];
  float material_emiss_rgb[3];
  float material_shine;
  float material_spec_rgb[3];
  int mesh;
  int mesh_num = 1;
  int object;
  float transform_forward[3];
  float transform_position[3];
  float transform_scale[3];
  float transform_up[3];
//
//  Make up some placeholder values for now.
//
  light_ambient_rgb[0] = 0.2;
  light_ambient_rgb[1] = 0.1;
  light_ambient_rgb[2] = 0.1;

  light_diffuse_rgb[0] = 0.1;
  light_diffuse_rgb[1] = 0.2;
  light_diffuse_rgb[2] = 0.1;

  light_direction[0] =   0.0;
  light_direction[1] =   0.0;
  light_direction[2] = 100.0;

  light_specular_rgb[0] = 0.1;
  light_specular_rgb[1] = 0.1;
  light_specular_rgb[2] = 0.2;

  material_alpha = 0.9;

  material_amb_rgb[0] = 0.1;
  material_amb_rgb[1] = 0.1;
  material_amb_rgb[2] = 0.1;

  material_diff_rgb[0] = 0.2;
  material_diff_rgb[1] = 0.1;
  material_diff_rgb[2] = 0.1;

  material_emiss_rgb[0] = 0.1;
  material_emiss_rgb[1] = 0.2;
  material_emiss_rgb[2] = 0.1;

  material_shine = 0.8;

  material_spec_rgb[0] = 0.1;
  material_spec_rgb[1] = 0.1;
  material_spec_rgb[2] = 0.2;

  transform_forward[0] = 0.0;
  transform_forward[1] = 0.0;
  transform_forward[2] = 0.0;

  transform_position[0] = 0.0;
  transform_position[1] = 0.0;
  transform_position[2] = 0.0;

  transform_scale[0] = 1.0;
  transform_scale[1] = 1.0;
  transform_scale[2] = 1.0;

  transform_up[0] = 1.0;
  transform_up[1] = 1.0;
  transform_up[2] = 1.0;

  object_num = 1;

  text_num = 0;

  fprintf ( fileout, "<WORLD>\n" );
  fprintf ( fileout, "\n" );

  text_num = text_num + 2;

  fprintf ( fileout, "  <BACKGROUND>\n" );
  fprintf ( fileout, "    <BACKCOLOR> %f, %f, %f </BACKCOLOR>\n",
    background_rgb[0], background_rgb[1], background_rgb[2] );
  fprintf ( fileout, "  </BACKGROUND>\n" );
  fprintf ( fileout, "\n" );
  fprintf ( fileout, "  <LIGHTING>\n" );
  fprintf ( fileout, "    <AMBIENT> %f, %f, %f </AMBIENT>\n",
    light_ambient_rgb[0], light_ambient_rgb[1], light_ambient_rgb[2] );
  fprintf ( fileout, "    <DIRECTIONALLIGHT>\n" );
  fprintf ( fileout, "      <DIFFUSE> %f, %f, %f </DIFFUSE>\n",
    light_diffuse_rgb[0], light_diffuse_rgb[1], light_diffuse_rgb[2] );
  fprintf ( fileout, "      <DIRECTION> %f, %f, %f </DIRECTION>\n",
    light_direction[0], light_direction[1], light_direction[2] );
  fprintf ( fileout, "      <SPECULAR> %f, %f, %f </SPECULAR>\n",
    light_specular_rgb[0], light_specular_rgb[1], light_specular_rgb[2] );
  fprintf ( fileout, "    </DIRECTIONALLIGHT>\n" );
  fprintf ( fileout, "  </LIGHTING>\n" );

  text_num = text_num + 12;

  for ( mesh = 0; mesh < mesh_num; mesh++ )
  {
    fprintf ( fileout, "\n" );
    fprintf ( fileout, "  <MESH ID = \"%d\">\n", mesh );
    fprintf ( fileout, "\n" );
    text_num = text_num + 3;

    for ( j = 0; j < cor3_num; j++ )
    {
      fprintf ( fileout, "    <P ID=\"%d\"> %f, %f, %f </P>\n", j,
        cor3[0][j], cor3[1][j], cor3[2][j] );
      text_num = text_num + 1;
    }

    fprintf ( fileout, "\n" );
    text_num = text_num + 1;
    for ( j = 0; j < cor3_num; j++ )
    {
      fprintf ( fileout, "    <N ID=\"%d\"> %f, %f, %f </N>\n", j,
        cor3_normal[0][j], cor3_normal[1][j], cor3_normal[2][j] );
      text_num = text_num + 1;
    }

    for ( material = 0; material < material_num; material++ )
    {
      fprintf ( fileout, "\n" );
      fprintf ( fileout, "    <MAT ID=\"%d\">\n", material );
      fprintf ( fileout, "      <ALPHA> %f </ALPHA>\n", material_alpha );
      fprintf ( fileout, "      <AMB> %f, %f, %f </AMB>\n",
        material_amb_rgb[0], material_amb_rgb[1], material_amb_rgb[2] );
      fprintf ( fileout, "      <DIFF> %f, %f, %f </DIFF>\n",
        material_diff_rgb[0], material_diff_rgb[1], material_diff_rgb[2] );
      fprintf ( fileout, "      <EMISS> %f, %f, %f </EMISS>\n",
        material_emiss_rgb[0], material_emiss_rgb[1], material_emiss_rgb[2] );
      fprintf ( fileout, "      <SHINE> %f </SHINE>\n", material_shine );
      fprintf ( fileout, "      <SPEC> %f, %f, %f </SPEC>\n",
        material_spec_rgb[0], material_spec_rgb[1], material_spec_rgb[2] );
      fprintf ( fileout, "    </MAT>\n" );
      text_num = text_num + 9;
    }

    fprintf ( fileout, "\n" );
    text_num = text_num + 1;

    for ( iface = 0; iface < face_num; iface++ )
    {
      fprintf ( fileout, "    <F>\n" );
      fprintf ( fileout, "      <MATREF> %d </MATREF>\n", face_material[iface] );
      text_num = text_num + 2;
      for ( ivert = 0; ivert < face_order[iface]; ivert++ )
      {
        fprintf ( fileout,
          "      <FV%d><PREF> %d </PREF><NREF> %d </NREF></FV%d>\n",
          ivert+1, face[ivert][iface], face[ivert][iface], ivert+1 );
        text_num = text_num + 1;
      }
      fprintf ( fileout, "    </F>\n" );
      text_num = text_num + 1;
    }

    fprintf ( fileout, "  </MESH>\n" );
    text_num = text_num + 1;

  }

  fprintf ( fileout, "\n" );
  text_num = text_num + 1;

  for ( object = 0; object < object_num; object++ )
  {
    fprintf ( fileout, "  <OBJECT>\n" );
    fprintf ( fileout, "    <TRANSFORM>\n" );
    fprintf ( fileout, "      <FORWARD> %f, %f, %f </FORWARD>\n",
      transform_forward[0], transform_forward[1], transform_forward[2] );
    fprintf ( fileout, "      <POSITION> %f, %f, %f </POSITION>\n",
      transform_position[0], transform_position[1], transform_position[2] );
    fprintf ( fileout, "'      <SCALE> %f, %f, %f </SCALE>\n",
      transform_scale[0], transform_scale[1], transform_scale[2] );
    fprintf ( fileout, "      <UP> %f, %f, %f </UP>\n",
      transform_up[0], transform_up[1], transform_up[2] );
    fprintf ( fileout, "    </TRANSFORM>\n" );
    mesh = 0;
    fprintf ( fileout, "    <MESHREF> %d </MESHREF>\n", mesh );
    fprintf ( fileout, "  </OBJECT>\n" );
    text_num = text_num + 9;
  }

  fprintf ( fileout, "\n" );
  fprintf ( fileout, "</WORLD>\n" );
  text_num = text_num + 2;

//
//  Report.
//
  cout << "\n";
  cout << "XGL_WRITE - Wrote " << text_num << " text lines.\n";

  return 0;
}
