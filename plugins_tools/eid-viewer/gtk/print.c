#include <config.h>
#include <cairo.h>
#include "gtk_globals.h"
#include "gtkui.h"
#include "glib_util.h"

#include "gettext.h"

#include <sys/utsname.h>

#ifndef _
#define _(s) gettext(s)
#endif

#ifndef N_
#define N_(s) gettext_noop(s)
#endif

static GtkPrintSettings *settings = NULL;

#define C_HEADER_HEIGHT 100.0
#define C_MARGIN 50.0
#define C_COL_SPACE 25.0

#define P_HEADER_HEIGHT (C_HEADER_HEIGHT * PANGO_SCALE)
#define P_MARGIN (C_MARGIN * PANGO_SCALE)
#define P_COL_SPACE (C_COL_SPACE * PANGO_SCALE)

static struct field {
	int group;
	char* printlabel;
	char* p11label;
} regularfields[] = {
	{ 1, N_("Name:"), "surname" },
	{ 1, N_("Given names:"), "firstnames" },
	{ 1, N_("Place of birth:"), "location_of_birth" },
	{ 1, N_("Date of birth:"), "date_of_birth" },
	{ 1, N_("Sex:"), "gender" },
	{ 1, N_("Nationality:"), "nationality" },
	{ 1, N_("National number:"), "national_number" },
	{ 2, N_("Title:"), "nobility" },
	{ 2, N_("Special status:"), "special_status" },
	{ 3, N_("Street:"), "address_street_and_number" },
	{ 3, N_("Postal code:"), "address_zip" },
	{ 3, N_("Municipality:"), "address_municipality" },
	{ 4, N_("Card number:"), "card_number" },
	{ 4, N_("Place of issue:"), "issuing_municipality" },
	{ 4, N_("Chip number:"), "chip_number" },
	{ 4, N_("Valid from:"), "validity_begin_date" },
	{ 4, N_("Valid until:"), "validity_end_date" },
};

static struct field foreignerfields[] = {
	{ 5, N_("Member of family:"), "member_of_family" },
	{ 5, N_("Special organization:"), "special_organization" },
	{ 5, N_("Duplicata:"), "duplicata" },
	{ 5, N_("Date and country of protection:"), "date_and_country_of_protection" },
	{ 5, N_("Work permit mention:"), "work_permit_type" },
	{ 5, N_("Employer's VAT number (1):"), "employer_vat_1" },
	{ 5, N_("Employer's VAT number (2):"), "employer_vat_2" },
	{ 5, N_("Regional file number:"), "regional_file_number" },
};

static struct field printfields[] = {
	{ 6, N_("Printed:"), "printdate" },
	{ 6, N_("Printed by:"), "printby" },
};

/* Show the photo on the right location on the page */
static void add_photo(cairo_t* cr, GdkPixbuf* buf, gdouble c_pagewidth, int right_side) {
	gdouble scalefactor = C_HEADER_HEIGHT / gdk_pixbuf_get_height(buf);

	cairo_save(cr);
	cairo_scale(cr, scalefactor, scalefactor);
	if(right_side) {
		gdk_cairo_set_source_pixbuf(cr, buf, c_pagewidth / scalefactor - C_MARGIN / scalefactor - gdk_pixbuf_get_width(buf), C_MARGIN / scalefactor);
	} else {
		gdk_cairo_set_source_pixbuf(cr, buf, C_MARGIN / scalefactor, C_MARGIN / scalefactor);
	}
	cairo_paint(cr);
	cairo_restore(cr);
	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
	cairo_move_to(cr, C_MARGIN, C_MARGIN + C_HEADER_HEIGHT);
	cairo_line_to(cr, c_pagewidth - C_MARGIN, C_MARGIN + C_HEADER_HEIGHT);
	cairo_stroke(cr);
}

/* Compute the width of the photo */
static gdouble get_photo_width(GdkPixbuf *buf) {
	gdouble scalefactor = C_HEADER_HEIGHT / gdk_pixbuf_get_height(buf);

	return gdk_pixbuf_get_width(buf) * scalefactor;
}

/* Called by the GtkPrintOperation when it wants us to draw a page */
static void draw_page(GtkPrintOperation* print, GtkPrintContext* context, gint page_nr, gpointer user_data) {
	cairo_t *cr;
	PangoLayout *header, *title, *data;
	gdouble c_pagewidth, c_pageheight, c_lineoffset, c_height,
		c_titlewidth, c_lineheight, c_headerwidth,
		c_headerspace, c_headerheight;
	gint p_pagewidth, p_pageheight, p_dataheight, p_titleheight,
	     p_datawidth, p_titlewidth, p_lineheight, p_headerwidth,
	     p_headerspace, p_headerheight;
	int i, j, lastgroup;
	int fieldlen = 2;
	int pointsize = 20;
	GdkPixbuf *photobuf, *coabuf;
	char today[20];
	gchar *viewver;
	time_t now = time(NULL);
	gchar* headertext;
	struct field* allfields[3];
	int sizes[3] = {
		sizeof(regularfields) / sizeof(struct field),
		sizeof(foreignerfields) / sizeof(struct field),
		sizeof(printfields) / sizeof(struct field),
	};
	struct utsname name;

	if(is_foreigner) {
		fieldlen = 3;
	}
	allfields[0] = regularfields;
	if(is_foreigner) {
		allfields[1] = foreignerfields;
	} else {
		sizes[1] = sizes[2];
	}
	allfields[fieldlen-1] = printfields;

	g_assert(page_nr == 0);

	/* Fill out data related to "today" and "this software" */
	strftime(today, 20, _("%B %d, %Y"), localtime(&now));
	uname(&name);
	viewver = g_strdup_printf("eID Viewer %s (%s)", PACKAGE_VERSION, name.sysname);
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "printby")), viewver);
	g_free(viewver);
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder, "printdate")), today);

	/* Initialization of various coordinate variables. GTK+ has two
	 * libraries used in printing: Cairo, a 2D graphics library, and
	 * Pango, a text rendering library. Unfortunately, they use a
	 * different coordinate system: Cairo uses double-precision
	 * floating point variables, Pango uses integer values. To not
	 * have to convert back and forth between the two, we have c_
	 * variables for Cairo coordinates, and p_ variables for Pango
	 * coordinates, and we bring them in sync as soon as is
	 * reasonable. */
	cr = gtk_print_context_get_cairo_context(context);
	c_pagewidth = gtk_print_context_get_width(context);
	c_pageheight = gtk_print_context_get_height(context);
	p_pagewidth = c_pagewidth * PANGO_SCALE;
	p_pageheight = c_pageheight * PANGO_SCALE;

	/* Initialize the pango layouts */
	title = gtk_print_context_create_pango_layout(context);
	data = gtk_print_context_create_pango_layout(context);
	header = gtk_print_context_create_pango_layout(context);

	pango_layout_set_alignment(title, PANGO_ALIGN_LEFT);
	pango_layout_set_width(title, -1);
	pango_layout_set_ellipsize(data, PANGO_ELLIPSIZE_NONE);

	pango_layout_set_alignment(data, PANGO_ALIGN_LEFT);
	pango_layout_set_wrap(data, PANGO_WRAP_WORD);
	pango_layout_set_ellipsize(data, PANGO_ELLIPSIZE_NONE);

	pango_layout_set_alignment(header, PANGO_ALIGN_CENTER);
	pango_layout_set_width(header, -1);
	pango_layout_set_ellipsize(header, PANGO_ELLIPSIZE_NONE);

	cairo_set_line_width(cr, 1.0);

	photobuf = gtk_image_get_pixbuf(GTK_IMAGE(gtk_builder_get_object(builder, "photo")));
	G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	switch(get_curlang()) {
		case EID_VWR_LANG_DE:
			coabuf = gdk_pixbuf_from_pixdata(&coat_of_arms_de, TRUE, NULL);
			break;
		case EID_VWR_LANG_FR:
			coabuf = gdk_pixbuf_from_pixdata(&coat_of_arms_fr, TRUE, NULL);
			break;
		case EID_VWR_LANG_NL:
			coabuf = gdk_pixbuf_from_pixdata(&coat_of_arms_nl, TRUE, NULL);
			break;
		default:
			coabuf = gdk_pixbuf_from_pixdata(&coat_of_arms_en, TRUE, NULL);
			break;
	}
	G_GNUC_END_IGNORE_DEPRECATIONS
	c_headerspace = c_pagewidth - get_photo_width(photobuf) - get_photo_width(coabuf);
	p_headerspace = c_headerspace * PANGO_SCALE;

	headertext = g_strdup_printf(_("BELGIUM\n%s"), gtk_label_get_text(GTK_LABEL(
					gtk_builder_get_object(builder, "document_type"))));

	/* Make sure the header text fits between the two pictures */
	do {
		gchar* fontname = g_strdup_printf("sans %d", pointsize--);
		PangoFontDescription *desc = pango_font_description_from_string(fontname);
		pango_layout_set_font_description(header, desc);
		pango_font_description_free(desc);
		g_free(fontname);

		pango_layout_set_text(header, headertext, -1);
		pango_layout_get_size(header, &p_headerwidth, &p_headerheight);
	} while(p_headerspace < p_headerwidth);
	
	c_headerwidth = p_headerwidth / PANGO_SCALE;
	c_headerheight = p_headerheight / PANGO_SCALE;

	pointsize++;

	/* Make sure the data fits on the page */
	do {
		gchar* fontname = g_strdup_printf("sans %d", pointsize--);
		PangoFontDescription *desc = pango_font_description_from_string(fontname);
		pango_layout_set_font_description(title, desc);
		pango_layout_set_font_description(data, desc);
		pango_font_description_free(desc);
		g_free(fontname);

		p_dataheight = p_titleheight = p_datawidth =
			p_titlewidth = p_lineheight = 0;

		/* First, loop over all the labels and find the widest one */
		for(i=0; i<fieldlen; i++) {
			for(j=0; j<sizes[i]; j++) {
				gint width, height;

				pango_layout_set_text(title, gettext(allfields[i][j].printlabel), -1);
				pango_layout_get_size(title, &width, &height);
				p_titlewidth = width > p_titlewidth ? width : p_titlewidth;
				p_lineheight = p_lineheight > height ? p_lineheight : height;
				p_titleheight += height;
			}
		}
		p_datawidth = p_pagewidth - P_MARGIN * 2 - P_COL_SPACE - p_titlewidth;
		pango_layout_set_width(data, p_datawidth);
		/* Now, loop over all the data elements and calculate each of their
		 * heights, after wordwrapping */
		for(i=0; i<fieldlen; i++) {
			for(j=0; j<sizes[i]; j++) {
				gint width, height;
				GObject *obj = gtk_builder_get_object(builder, allfields[i][j].p11label);

				if(GTK_IS_LABEL(obj)) {
					pango_layout_set_text(data, gtk_label_get_text(GTK_LABEL(obj)), -1);
				} else { // checkbox
					if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(obj))) {
						pango_layout_set_text(data, "🗸", -1);
					} else {
						pango_layout_set_text(data, "-", -1);
					}
				}
				pango_layout_get_size(data, &width, &height);

				if(width > p_datawidth) {
					/* word-wrapping was impossible, we don't fit;
					 * ensure we don't fit vertically too, so the
					 * while condition triggers and we try again
					 * with a smaller font */
					p_dataheight += p_pageheight;
					break;
				}
				p_dataheight += height;
			}
		}
	} while(p_dataheight > (p_pageheight - P_HEADER_HEIGHT - p_lineheight * 4 - P_MARGIN * 2)
		       	|| p_titleheight > (p_pageheight - P_HEADER_HEIGHT - p_lineheight * 4 - P_MARGIN * 2));

	/* We've scaled everything now -- actually render it on the page */
	c_height = (gdouble)(p_dataheight > p_titleheight ? p_dataheight : p_titleheight) / PANGO_SCALE;
	c_lineheight = (gdouble)p_lineheight / PANGO_SCALE;

	c_lineoffset = C_MARGIN + C_HEADER_HEIGHT + (c_pageheight - C_HEADER_HEIGHT - c_height - C_MARGIN * 2) / 2;
	c_titlewidth = (gdouble)p_titlewidth / PANGO_SCALE;
	lastgroup = 1;
	for(i=0; i<fieldlen; i++) {
		for(j=0;j<sizes[i]; j++) {
			gint p_dh, p_th;
			gdouble c_h;
			GObject *obj = gtk_builder_get_object(builder, allfields[i][j].p11label);

			if(allfields[i][j].group > lastgroup) {
				lastgroup = allfields[i][j].group;
				/* draw a line */
				cairo_move_to(cr, C_MARGIN, c_lineoffset + c_lineheight / 2);
				cairo_line_to(cr, c_pagewidth - C_MARGIN, c_lineoffset + c_lineheight / 2);
				cairo_stroke(cr);
				c_lineoffset += c_lineheight;
			}

			pango_layout_set_text(title, gettext(allfields[i][j].printlabel), -1);
			if(GTK_IS_LABEL(obj)) {
				pango_layout_set_text(data, gtk_label_get_text(GTK_LABEL(obj)), -1);
			} else {
				if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(obj))) {
					pango_layout_set_text(data, "🗸", -1);
				} else {
					pango_layout_set_text(data, "-", -1);
				}
			}
			cairo_move_to(cr, C_MARGIN, c_lineoffset);
			pango_cairo_show_layout(cr, title);
			cairo_move_to(cr, C_MARGIN + c_titlewidth + C_COL_SPACE, c_lineoffset);
			pango_cairo_show_layout(cr, data);

			pango_layout_get_size(title, NULL, &p_th);
			pango_layout_get_size(data, NULL, &p_dh);
			c_h = (gdouble)(p_dh > p_th ? p_dh : p_th) / PANGO_SCALE;

			c_lineoffset += c_h;
		}
	}
	add_photo(cr, photobuf, c_pagewidth, 1);
	add_photo(cr, coabuf, c_pagewidth, 0);

	cairo_move_to(cr, (c_pagewidth / 2) - (c_headerwidth / 2), C_MARGIN + (C_HEADER_HEIGHT /2) - (c_headerheight / 2));
	pango_cairo_show_layout(cr, header);
}

/* Called by UI when the user activates the "Print" menu item */
void do_print(GtkMenuItem* item G_GNUC_UNUSED, gpointer user_data G_GNUC_UNUSED) {
	GtkPrintOperation *print = gtk_print_operation_new();
	GtkPrintOperationResult res;
	gchar* name;

	if(settings != NULL) {
		gtk_print_operation_set_print_settings(print, settings);
	}
	g_signal_connect(print, "draw_page", G_CALLBACK(draw_page), NULL);

	name = g_strdup_printf("eID-Viewer: %s %s (%s)", 
			gtk_label_get_text(GTK_LABEL(gtk_builder_get_object(builder, "firstnames"))),
			gtk_label_get_text(GTK_LABEL(gtk_builder_get_object(builder, "surname"))),
			gtk_label_get_text(GTK_LABEL(gtk_builder_get_object(builder, "national_number"))));
	gtk_print_operation_set_job_name(print, name);
	g_free(name);

	gtk_print_operation_set_n_pages(print, 1);

	res = gtk_print_operation_run(print, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG,
			GTK_WINDOW(gtk_builder_get_object(builder, "mainwin")), NULL);

	if(res == GTK_PRINT_OPERATION_RESULT_APPLY) {
		if(settings != NULL) {
			g_object_unref(settings);
		}
		settings = g_object_ref(gtk_print_operation_get_print_settings(print));
	}

	g_object_unref(print);
}
