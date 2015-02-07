#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLUSTER_SIZE        0x200
#define CRC_MAX_BLOCK_SIZE  0x80000

#define BYTEn(x, n)         ( *( (_BYTE*) &(x) + n ) )
#define BYTE1(x)            BYTEn(x, 1)
#define _BYTE               unsigned char

int AppendCRC( const char *filename );
int GetBlockCRC( int data_size, void *data_array, int prev_crc );

int AppendCRC( const char *filename )
{
   FILE *fw_file;
   int filepos_begin;
   int filepos_end;

   void *data_array;
   int data_size;

   int return_code = 0;
   int crc = 0;

   fw_file = fopen( filename, "rb+" );
   if( !fw_file )
   {
      printf( "add crc open file error.\n" );
      return -1;
   }

   filepos_begin = 0;
   fseek( fw_file, 0, SEEK_END );
   filepos_end = ftell( fw_file );

   fseek( fw_file, 0, SEEK_SET );
   data_array = malloc( CRC_MAX_BLOCK_SIZE );
   if ( data_array )
   {
      while( filepos_begin < filepos_end )
      {
         data_size = filepos_end - filepos_begin < CRC_MAX_BLOCK_SIZE? filepos_end - filepos_begin: CRC_MAX_BLOCK_SIZE;
         if( fread( data_array, data_size, 1, fw_file ) != 1 )
         {
            printf( "add crc, read file error\n" );
            return_code = -3;
            free( data_array );

            fclose( fw_file );
            return return_code;
         }

         crc = GetBlockCRC( data_size, data_array, crc );
         memset( data_array, 0, CRC_MAX_BLOCK_SIZE );
         filepos_begin += data_size;
      } 

      printf( "crc=0x%08x\n", crc );
      free( data_array );
   }

   fseek( fw_file, 0, SEEK_END );
   if( fwrite( &crc, sizeof( crc ), 1, fw_file ) != 1 )
   {
      printf( "add crc write failed !!!\n" );
      return_code = -4;
   }

   fclose( fw_file );
   return return_code;
}

int GetBlockCRC( int data_size, void *data_array, int prev_crc )
{
   int i = 0;
   int values[256] = { 
      0, 4129, 8258, 12387, 16516, 20645, 24774, 28903, 33032, 37161, 41290, 45419, 49548, 53677, 57806, 61935, 4657, 528, 12915, 8786, 21173, 17044, 29431, 25302, 37689, 33560, 45947, 41818, 54205, 50076, 62463, 58334, 9314, 13379, 1056, 5121, 25830, 29895, 17572, 21637, 42346, 46411, 34088, 38153, 58862, 62927, 50604, 54669, 13907, 9842, 5649, 1584, 30423, 26358, 22165, 18100, 46939, 42874, 38681, 34616, 63455, 59390, 55197, 51132, 18628, 22757, 26758, 30887, 2112, 6241, 10242, 14371, 51660, 55789, 59790, 63919, 35144, 39273, 43274, 47403, 23285, 19156, 31415, 27286, 6769, 2640, 14899, 10770, 56317, 52188, 64447, 60318, 39801, 35672, 47931, 43802, 27814, 31879, 19684, 23749, 11298, 15363, 3168, 7233, 60846, 64911, 52716, 56781, 44330, 48395, 36200, 40265, 32407, 28342, 24277, 20212, 15891, 11826, 7761, 3696, 65439, 61374, 57309, 53244, 48923, 44858, 40793, 36728, 37256, 33193, 45514, 41451, 53516, 49453, 61774, 57711, 4224, 161, 12482, 8419, 20484, 16421, 28742, 24679, 33721, 37784, 41979, 46042, 49981, 54044, 58239, 62302, 689, 4752, 8947, 13010, 16949, 21012, 25207, 29270, 46570, 42443, 38312, 34185, 62830, 58703, 54572, 50445, 13538, 9411, 5280, 1153, 29798, 25671, 21540, 17413, 42971, 47098, 34713, 38840, 59231, 63358, 50973, 55100, 9939, 14066, 1681, 5808, 26199, 30326, 17941, 22068, 55628, 51565, 63758, 59695, 39368, 35305, 47498, 43435, 22596, 18533, 30726, 26663, 6336, 2273, 14466, 10403, 52093, 56156, 60223, 64286, 35833, 39896, 43963, 48026, 19061, 23124, 27191, 31254, 2801, 6864, 10931, 14994, 64814, 60687, 56684, 52557, 48554, 44427, 40424, 36297, 31782, 27655, 23652, 19525, 15522, 11395, 7392, 3265, 61215, 65342, 53085, 57212, 44955, 49082, 36825, 40952, 28183, 32310, 20053, 24180, 11923, 16050, 3793, 7920
   };

   int result = prev_crc;
   for(i = 0; i < data_size; ++i )
   {
      result = *( values + ( *(_BYTE *) data_array ^ BYTE1(result) ) ) ^ (result << 8);
      data_array = (_BYTE *) data_array + 1;
   }
   return result;
}
