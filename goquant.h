#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rwpng.h"
#include "libimagequant.h"

static void set_palette(liq_result *result, png8_image *output_image)
{
    const liq_palette *palette = liq_get_palette(result);

    output_image->num_palette = palette->count;
    for(unsigned int i=0; i < palette->count; i++) {
        liq_color px = palette->entries[i];
        output_image->palette[i] = (rwpng_rgba){.r=px.r, .g=px.g, .b=px.b, .a=px.a};
    }
}

static pngquant_error prepare_output_image(liq_result *result, liq_image *input_image, rwpng_color_transform output_color, png8_image *output_image)
{
    output_image->width = liq_image_get_width(input_image);
    output_image->height = liq_image_get_height(input_image);
    output_image->gamma = liq_get_output_gamma(result);
    output_image->output_color = output_color;

    /*
    ** Step 3.7 [GRR]: allocate memory for the entire indexed image
    */

    output_image->indexed_data = malloc(output_image->height * output_image->width);
    output_image->row_pointers = malloc(output_image->height * sizeof(output_image->row_pointers[0]));

    if (!output_image->indexed_data || !output_image->row_pointers) {
        return OUT_OF_MEMORY_ERROR;
    }

    for(size_t row = 0; row < output_image->height; row++) {
        output_image->row_pointers[row] = output_image->indexed_data + row * output_image->width;
    }

    const liq_palette *palette = liq_get_palette(result);
    // tRNS, etc.
    output_image->num_palette = palette->count;

    return SUCCESS;
}
char *custom_create(void *file_cont, int sz, size_t *out_length) {
    liq_image *input_image = NULL;
    liq_result *res = NULL;
    int result;

    png24_image input_image_rw = {};
    png24_image *input_image_rwpng = &input_image_rw;

    png8_image output_image = {};

    liq_attr *attr = liq_attr_create();
    liq_set_quality(attr, 80, 90);

    rwpng_read_image24_libpng(file_cont, input_image_rwpng, 0, 0);

    input_image = liq_image_create_rgba_rows(
            attr, (void **)input_image_rwpng->row_pointers, 
            input_image_rwpng->width, 
            input_image_rwpng->height, input_image_rwpng->gamma);

    liq_error err = liq_image_quantize(input_image, attr, &res);

    prepare_output_image(
            res, input_image, input_image_rwpng->output_color, &output_image);

    liq_error err2 = liq_write_remapped_image_rows(res, input_image, output_image.row_pointers);

    set_palette(res, &output_image);

    dynamic_buf* out_image_buf = create_image_buf_len(sz);

    result = rwpng_write_image8_to_buf(out_image_buf, &output_image);

    if (result != SUCCESS)
    {
        fprintf(stderr, "Error occured while writing to file : %d", result);
        exit(EXIT_FAILURE);
    }

    *out_length = out_image_buf->length;

    return out_image_buf->buffer;
}
