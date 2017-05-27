#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/hiddev.h>

void showReports(int fd, unsigned report_type)
{
	struct hiddev_report_info rinfo;
	struct hiddev_field_info finfo;
	struct hiddev_usage_ref uref;
    unsigned int i, j;
    int ret;

	rinfo.report_type = report_type;
	rinfo.report_id = HID_REPORT_ID_FIRST;
	ret = ioctl(fd, HIDIOCGREPORTINFO, &rinfo);
	while (ret >= 0)
	{
		printf("HIDIOCGREPORTINFO: report_id=0x%X (%u fields)\n",rinfo.report_id, rinfo.num_fields);
		for (i = 0; i < rinfo.num_fields; i++)
		{
			finfo.report_type = rinfo.report_type;
			finfo.report_id = rinfo.report_id;
			finfo.field_index = i;
			ioctl(fd, HIDIOCGFIELDINFO, &finfo);

			printf(
					"HIDIOCGFIELDINFO: field_index=%u maxusage=%u flags=0x%X\n"
							"\tphysical=0x%X logical=0x%X application=0x%X\n"
							"\tlogical_minimum=%d,maximum=%d physical_minimum=%d,maximum=%d\n",
					finfo.field_index, finfo.maxusage, finfo.flags,
					finfo.physical, finfo.logical, finfo.application,
					finfo.logical_minimum, finfo.logical_maximum,
					finfo.physical_minimum, finfo.physical_maximum);

			for (j = 0; j < finfo.maxusage; j++)
			{
				uref.report_type = finfo.report_type;
				uref.report_id = finfo.report_id;
				uref.field_index = i;
				uref.usage_index = j;
				ioctl(fd, HIDIOCGUCODE, &uref);
				ioctl(fd, HIDIOCGUSAGE, &uref);

				printf(" >> usage_index=%u usage_code=0x%X () value=%d\n",
						uref.usage_index, uref.usage_code,
						//    controlName(uref.usage_code),
						uref.value);

			}
		}
		printf("\n");

		rinfo.report_id |= HID_REPORT_ID_NEXT;
		ret = ioctl(fd, HIDIOCGREPORTINFO, &rinfo);
	}
}

void show_all_report(int fd)
{

	struct hiddev_report_info rinfo;
	struct hiddev_field_info finfo;
	struct hiddev_usage_ref uref;
    unsigned int rtype, i, j;
	char *rtype_str;

	for (rtype = HID_REPORT_TYPE_MIN; rtype <= HID_REPORT_TYPE_MAX; rtype++)
	{
		switch (rtype)
		{
		case HID_REPORT_TYPE_INPUT:
			rtype_str = "Input";
			break;
		case HID_REPORT_TYPE_OUTPUT:
			rtype_str = "Output";
			break;
		case HID_REPORT_TYPE_FEATURE:
			rtype_str = "Feature";
			break;
		default:
			rtype_str = "Unknown";
			break;
		}
		fprintf(stdout, "Reports of type %s (%d):\n", rtype_str, rtype);
		rinfo.report_type = rtype;
		rinfo.report_id = HID_REPORT_ID_FIRST;
		while (ioctl(fd, HIDIOCGREPORTINFO, &rinfo) >= 0)
		{
			fprintf(stdout, " Report id: %d (%d fields)\n", rinfo.report_id,
					rinfo.num_fields);
			for (i = 0; i < rinfo.num_fields; i++)
			{
				memset(&finfo, 0, sizeof(finfo));
				finfo.report_type = rinfo.report_type;
				finfo.report_id = rinfo.report_id;
				finfo.field_index = i;
				ioctl(fd, HIDIOCGFIELDINFO, &finfo);
				fprintf(stdout, " Field: %d: app: %04x phys %04x "
						"flags %x (%d usages) unit %x exp %d\n", i,
						finfo.application, finfo.physical, finfo.flags,
						finfo.maxusage, finfo.unit, finfo.unit_exponent);
				memset(&uref, 0, sizeof(uref));
				for (j = 0; j < finfo.maxusage; j++)
				{
					uref.report_type = finfo.report_type;
					uref.report_id = finfo.report_id;
					uref.field_index = i;
					uref.usage_index = j;
					ioctl(fd, HIDIOCGUCODE, &uref);
					ioctl(fd, HIDIOCGUSAGE, &uref);
					fprintf(stdout, " Usage: %04x val %d\n", uref.usage_code,
							uref.value);
				}
			}
			rinfo.report_id |= HID_REPORT_ID_NEXT;
		}
	}
	// if (!run_as_daemon)
	fprintf(stdout, "Waiting for events ... (interrupt to exit)\n");

}

const unsigned char tab[41] =
{
    0,0,0,0,65,66,67,68,69,70,
    71,72,73,74,75,76,77,78,79,80,
    81,82,83,84,85,86,87,88,89,90,
    49,50,51,52,53,54,55,56,57,48,
    0
};

