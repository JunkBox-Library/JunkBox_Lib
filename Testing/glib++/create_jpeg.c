#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <jpeglib.h>

int main()
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *outfile;

    JSAMPARRAY img;
    int i, j;

    // イメージを保持するメモリ領域の確保と初期化
    img = (JSAMPARRAY)malloc( sizeof( JSAMPROW ) * 256 );
    for ( i = 0; i < 256; i++ ) {
        img[i] = (JSAMPROW)malloc( sizeof( JSAMPLE ) * 3 * 256 );
        for ( j = 0; j < 256; j++ ) {
            img[i][ j * 3 + 0 ] = i;
            img[i][ j * 3 + 1 ] = 0;
            img[i][ j * 3 + 2 ] = j;
        }
    }

    // JPEGオブジェクトの初期化
    cinfo.err = jpeg_std_error( &jerr );
    jpeg_create_compress( &cinfo );

    // ファイルを開く
    outfile = fopen( "a.jpg", "wb" );
    jpeg_stdio_dest( &cinfo, outfile );

    // パラメータの設定
    cinfo.image_width = 256;
    cinfo.image_height = 256;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    // デフォルト値の設定
    jpeg_set_defaults( &cinfo );

    // 圧縮の開始
    jpeg_start_compress( &cinfo, TRUE );

    // 全イメージデータを出力
    jpeg_write_scanlines( &cinfo, img, 256 );

    // 圧縮の終了
    jpeg_finish_compress( &cinfo );

    // JPEGオブジェクトの破棄
    jpeg_destroy_compress( &cinfo );

    // ファイルを閉じる
    fclose( outfile );  

    // イメージデータを保持するメモリ領域を開放
    for ( i = 0; i < 256; i++ )
        free( img[i] );
    free( img );
}


