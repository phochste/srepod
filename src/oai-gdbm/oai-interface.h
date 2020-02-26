/* 
 * Authors: Patrick Hochstenbach <Patrick.Hochstenbach@UGent.be>
 *
 * $Id: oai-interface.h,v 1.2 2006/12/19 08:33:16 hochstenbach Exp $
 * 
 */
#ifndef	__oai_interface_h
#define __oai_interface_h

/* oai_interface_version - contains a version number for this interface */
char	*oai_interface_version;

/* oai_open_repository - this function should return 0 if the OAI repository
 * is available or returns a negative value on failure. The caller MUST NOT use any of
 * the following functions if this call returns not zero. 
 */
int	oai_open_repository(void);

/* oai_close_repository - this function should close the OAI repository
 * and return 0 on success or -1 on failure.
 */
int	oai_close_repository(void);

/* oai_progname - returns the baseURL of the OAI repository as char *. */
char    *oai_progname(void);

/* oai_granularity - returns the granularity of datestamp as an integer .
 *	
 *	return 0  if the granularity is DAY
 *	return 1  if the granularity is SECONDS
 *      return -1 on failure
 */
int     oai_granularity(void);

/* oai_num_of_sets - returns the number of sets available in the OAI repository
 * or return 0 on failure.
 */
int     oai_num_of_sets(void);

/* oai_identifier_exists - returns 1 if the specified identifier exists in the
 * OAI repository, returns 0 otherwise. If metadataPrefix is not NULL, then this
 * function returns 1 if the specified can be disseminated in this metadata format,
 * and returns 0 otherwise.
 */
int     oai_identifier_exists(const char *identifier, const char *metadataPrefix);

/* oai_num_metadataformats - returns the number of metadata formats in which
 * the specified identifier can be disseminated.
 */
int	oai_num_metadataformats(const char *identifier);

/* oai_valid_token - returns 1 if the resumptionToken exists, return 0 otherwhise. */
int	oai_valid_token(const char *resumptionToken);

/* oai_valid_format - returns 1 is the metadataPrefix is valid for this OAI repository,
 * returns 0 otherwhise. 
 */
int	oai_valid_format(const char *metadataPrefix);

/* oai_identify - prints a Identify reponse to stdout, returns the number of bytes
 * written.
 */
int     oai_identify(void);

/* oai_listmetadataformats - prints a ListMetadataFormats reponse to stdout, returns
 * the number of bytes written. Optinally an identifier can be specified. In this case
 * the function should only print the metadata formats available for this identifier.
 * The function should savely assume that the identifier exists in the OAI repository.
 */
int	oai_listmetadataformats(const char *identifier);

/* oai_listsets - prints a ListSets reponse to stdout, returns the number of bytes
 * written. Optionally an resumptionToken can be specified. The function should 
 * savely assume that the resumptionToken is valid in the OAI repository.
 */
int	oai_listsets(const char *resumptionToken);

/* oai_list - given the value of the return_type, this function should do the following:
 *
 *  return_type = 0 : Return the number of items that match the {set,metadataPrefix,from,until}
 *  		      or {resumptionToken} parameters.
 *  return_type = 1 : Print to stdout a response to ListIdentifiers with the parameters
 *  		      {set,metadataPrefix,from,until} or {resumptionToken} and return the
 *  		      number of bytes written.
 *  return_type = 2 : Print to stdout a response to ListRecords with th parameters
 *  		      {set,metadataPrefix,from,until} or {resumptionToken} and return the
 *  		      number of bytes writtem.
 *
 * The function should assume that all not-NULL parameters have legal values for this
 * OAI repository.
 */  		      
int	oai_list(const char *set,const char *metadataPrefix, const char *from, const char *until, const char *resumptionToken, int return_type);

/* oai_get - print a GetRecord reponse to stdout given an identifier and metadataPrefix.
 * This function returns the nymber of bytes written. This function should assume that
 * all parameters have legal values for this OAI repository.
 */
int	oai_get(const char *identifier, const char *metadataPrefix);

#endif
