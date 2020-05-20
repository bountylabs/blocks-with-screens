import csv
import sys

fname = sys.argv[1]

active_designators = {}
with open(fname+'.csv') as csvfile:
    csvreader = csv.reader(csvfile, delimiter=';', quotechar='"')
    outcsv = open(fname+'_bom.csv', 'w')
    csvwriter = csv.writer(outcsv, delimiter=',', quotechar='"')

    itemnum = 0
    for row in csvreader:
        if itemnum == 0:
            # header row
            outrow = []
            outrow.append("Item #")
            outrow.append("Designator")
            outrow.append("Qty")
            outrow.append("Manufacturer")
            outrow.append("Mfg Part #")
            outrow.append("Description / Value")
            outrow.append("Package/Footprint")
            outrow.append("Type")
            outrow.append("Your Instructions / Notes")

            #print outrow
            csvwriter.writerow(outrow)
            itemnum += 1

        elif itemnum > 0:
            print(', '.join(row))
            print len(row)
            # check if item is populated
            pop = row[7]
            # remember designators to assemble
            designator_list = row[4]
            designators = designator_list.split(", ")

            if pop != "0":
                for designator in designators:
                    active_designators[designator] = True

                outrow = []
                outrow.append(itemnum)  # item number
                outrow.append(row[4])   # designator
                outrow.append(row[0])   # quantity
                outrow.append("")       # manufacturer
                outrow.append(row[6])   # manufacturer's part number
                outrow.append(row[5])   # description
                outrow.append(row[3])   # package
                outrow.append("SMD")       # type (smd, thru-hole, etc)
                outrow.append("")       # notes

                #print outrow
                csvwriter.writerow(outrow)
                itemnum += 1
            else:
                for designator in designators:
                    active_designators[designator] = False

# pick and place files
# output for pnp file
outcsv = open(fname+'_pnp.csv', 'w')
csvwriter = csv.writer(outcsv, delimiter=',', quotechar='"')

# header
outrow = []
outrow.append("Designator")
outrow.append("Footprint")
outrow.append("Mid X")
outrow.append("Mid Y")
outrow.append("Ref X")
outrow.append("Ref Y")
outrow.append("Pad X")
outrow.append("Pad Y")
outrow.append("TB")
outrow.append("Rotation")
outrow.append("Comment")

csvwriter.writerow(outrow)

with open('PnP_'+fname+'_front.csv') as csvfile:
    csvreader = csv.reader(csvfile, delimiter=',', quotechar='"')

    tb = "T"
    for row in csvreader:
        designator = row[0]
        if active_designators[designator] == True:
            footprint = row[5]
            posx = row[1]
            posy = row[2]
            rotation = row[3]
            comment = row[4]

            outrow = []
            outrow.append(designator)   # Designator
            outrow.append(footprint)    # Footprint
            outrow.append(posx)         # Mid X
            outrow.append(posy)         # Mid Y
            outrow.append(posx)         # Ref X
            outrow.append(posy)         # Ref Y
            outrow.append(posx)         # Pad X
            outrow.append(posy)         # Pad Y
            outrow.append(tb)           # TB (top or bottom)
            outrow.append(rotation)     # Rotation
            outrow.append(comment)      # Comment

            csvwriter.writerow(outrow)

with open('PnP_'+fname+'_back.csv') as csvfile:
    csvreader = csv.reader(csvfile, delimiter=',', quotechar='"')

    tb = "B"
    for row in csvreader:
        designator = row[0]
        if active_designators[designator] == True:
            footprint = row[5]
            posx = row[1]
            posy = row[2]
            rotation = row[3]
            comment = row[4]

            outrow = []
            outrow.append(designator)   # Designator
            outrow.append(footprint)    # Footprint
            outrow.append(posx)         # Mid X
            outrow.append(posy)         # Mid Y
            outrow.append(posx)         # Ref X
            outrow.append(posy)         # Ref Y
            outrow.append(posx)         # Pad X
            outrow.append(posy)         # Pad Y
            outrow.append(tb)           # TB (top or bottom)
            outrow.append(rotation)     # Rotation
            outrow.append(comment)      # Comment

            csvwriter.writerow(outrow)
