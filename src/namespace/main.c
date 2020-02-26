/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: main.c,v 1.2 2006/12/19 08:33:16 hochstenbach Exp $
 * 
 */
/* 
 * NAMESPACE
 *
 * This program filters a static repository file from in input so
 * that all namespace declarations and schemaLocations defined in
 * elements outside the record element, but used inside record elements,
 * are written in the record element itself.
 * E.g. if we have an input file structured like this:
 *
 * <Repository 
 *          xmlns="http://www.openarchives.org/OAI/2.0/static-repository" 
 *          xmlns:x="http://info.internet.isi.edu:80/in-notes/rfc/files/rfc1807.txt" 
 *          xsi:schemaLocation="http://www.openarchives.org/OAI/2.0/static-repository 
 *                              http://www.openarchives.org/OAI/2.0/static-repository.xsd
 *                              http://info.internet.isi.edu:80/in-notes/rfc/files/rfc1807.txt
 *                              http://www.openarchives.org/OAI/1.1/rfc1807.xsd">
 * <Identify>...</Identify>
 * <ListMetadataFormats>...</ListMetadataFormats>
 * <ListRecords metadataPrefix="oai_rfc1807">
 *   <oai:record>
 *   <oai:header>...</oai:header>
 *   <oai:metadata>
 *     <x:rfc1807>
 *      .
 *      .
 *      .
 *     </x:rfc1807>
 *   </oai:metadata>
 * </ListRecords>
 * </Repository>
 *
 * then the output should transpose all namespace declarations and schemaLocations needed
 * for prefix 'x' to the 'rfc1807' element:
 *
 * <Repository 
 *          xmlns="http://www.openarchives.org/OAI/2.0/static-repository" 
 *          xsi:schemaLocation="http://www.openarchives.org/OAI/2.0/static-repository 
 *                              http://www.openarchives.org/OAI/2.0/static-repository.xsd">
 * <Identify>...</Identify>
 * <ListMetadataFormats>...</ListMetadataFormats>
 * <ListRecords metadataPrefix="oai_rfc1807">
 *   <oai:record>
 *   <oai:header>...</oai:header>
 *   <oai:metadata>
 *     <x:rfc1807
 *          xmlns:x="http://info.internet.isi.edu:80/in-notes/rfc/files/rfc1807.txt" 
 *          xsi:schemaLocation="http://info.internet.isi.edu:80/in-notes/rfc/files/rfc1807.txt
 *                              http://www.openarchives.org/OAI/1.1/rfc1807.xsd"
 *     >
 *      .
 *      .
 *      .
 *     </x:rfc1807>
 *   </oai:metadata>
 * </ListRecords>
 * </Repository>
 *
 * HOW THIS IS DONE
 *
 * We parse the input XML file twice with the subroutines scan1 and scan2. Scan1 
 * reads the XML file and creates a stack of hashes containing all the namespaces
 * found in the records for each ListRecords element. Scan1 also creates a hash
 * of all namespace uri/schemaLocation combinations found in elements outside
 * the records. Based on these two memory structures, the scan2 subroutine can
 * fill in the right declarations at the right place.
 *
 * REMARKS
 *
 * This code will not transpose declarations made in the 'Identify' section of
 * the input XML document.
 *
 * If about containers are made which contain metadata from several different
 * namespaces, then the transposed version will contain all declared namespaces
 * and schemaLocations. These could potentially clash when namespace prefixes
 * get redefined.
 *
 * We also try to scan the XML for potentional buffer overflow probs
 * we may encounter in subsequent transformtion programs.
 *
 * Author: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: main.c,v 1.2 2006/12/19 08:33:16 hochstenbach Exp $
 * 
 */

#include "defs.h"

char	*pname;

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage : %s file\n", argv[0]);
		exit(1);
	}
	
	pname = argv[0];

	outHash 	= hash_new();
	schemaHash	= hash_new();
	prefixStack     = create_stack(256);

	scan1(argv[1]);
	scan2(argv[1]);

  	hash_free(outHash);
	hash_free(schemaHash);
	destroy_stack(prefixStack);

	return 0;
}
