/*
 * Copyright (c) 2018-2020 Atmosphère-NX
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include <vapours/results/results_common.hpp>

namespace ams::npns {

    R_DEFINE_NAMESPACE_RESULT_MODULE(154);

    R_DEFINE_ERROR_RESULT(_100, 100);
    R_DEFINE_ERROR_RESULT(_101, 101);
    R_DEFINE_ERROR_RESULT(NotAuthenticated, 102);
    R_DEFINE_ERROR_RESULT(InvalidInput, 105);
    R_DEFINE_ERROR_RESULT(NotFound, 106);
    R_DEFINE_ERROR_RESULT(JidNotSet, 107);
    R_DEFINE_ERROR_RESULT(JidAlreadySet, 108);

    R_DEFINE_ERROR_RESULT(_410, 410);

    /* converted from stanza error */
    R_DEFINE_ERROR_RANGE(XmppError, 500, 599);
    R_DEFINE_ERROR_RESULT(XmppOutOfMemory, 501);           // XMPP_EMEM
    R_DEFINE_ERROR_RESULT(XmppInvalidInput, 502);          // XMPP_EINVOP
    R_DEFINE_ERROR_RESULT(XmppNotFound, 504);              // item-not-found
    R_DEFINE_ERROR_RESULT(XmppUnexpectedInput, 505);       // unexpected-request
    R_DEFINE_ERROR_RESULT(XmppNotAcceptable, 506);         // not-acceptable
    R_DEFINE_ERROR_RESULT(XmppBadFormat, 507);             // bad-format
    R_DEFINE_ERROR_RESULT(XmppBadRequest, 508);            // bad-request
    R_DEFINE_ERROR_RESULT(XmppConflict, 509);              // conflict
    R_DEFINE_ERROR_RESULT(XmppFeatureNotImplemented, 510); // feature-not-implemented
    R_DEFINE_ERROR_RESULT(XmppForbidden, 511);             // forbidden
    R_DEFINE_ERROR_RESULT(XmppGone, 512);                  // gone
    R_DEFINE_ERROR_RESULT(XmppInternalServerError, 513);   // internal-server-error
    R_DEFINE_ERROR_RESULT(XmppJidMalformed, 514);          // jid-malformed
    R_DEFINE_ERROR_RESULT(XmppNotAllowed, 515);            // not-allowed
    R_DEFINE_ERROR_RESULT(XmppNotAuthorized, 516);         // not-authorized
    R_DEFINE_ERROR_RESULT(XmppPaymentRequired, 517);       // payment-required
    R_DEFINE_ERROR_RESULT(XmppRecipientUnavailable, 518);  // recipient-unavailable
    R_DEFINE_ERROR_RESULT(XmppRedirect, 519);              // redirect
    R_DEFINE_ERROR_RESULT(XmppRegistrationRequired, 520);  // registration-required
    R_DEFINE_ERROR_RESULT(XmppRemoveServerNotFound, 521);  // remote-server-not-found
    R_DEFINE_ERROR_RESULT(XmppRemoveServerTimeout, 522);   // remote-server-timeout
    R_DEFINE_ERROR_RESULT(XmppResourceConstraint, 524);    // resource-constraint
    R_DEFINE_ERROR_RESULT(XmppServiceUnavailable, 525);    // service-unavailable
    R_DEFINE_ERROR_RESULT(XmppSubscriptionRequired, 526);  // subscription-required
    R_DEFINE_ERROR_RESULT(XmppUnknownError, 599);

    /* forgot */
    R_DEFINE_ERROR_RANGE(UnknownError, 600, 699);
    R_DEFINE_ERROR_RESULT(UnknownForbidden, 601);           // 403
    R_DEFINE_ERROR_RESULT(UnknownInternalServerError, 602); // 500

    R_DEFINE_ERROR_RESULT(_1023, 1023);
    R_DEFINE_ERROR_RESULT(_1024, 1024);

}

namespace ams::xmpp {

    R_DEFINE_NAMESPACE_RESULT_MODULE(155);

    R_DEFINE_ERROR_RESULT(_CURL_8001, 8001); // Unsupported protocol
    R_DEFINE_ERROR_RESULT(_CURL_8002, 8002); // Failed initialization
    R_DEFINE_ERROR_RESULT(_CURL_8003, 8003); // URL using bad/illegal format or missing URL
    R_DEFINE_ERROR_RESULT(_CURL_8004, 8004); // A requested feature, protocol or option was not found built-in in this libcurl due to a build-time decision.
    R_DEFINE_ERROR_RESULT(_CURL_8005, 8005); // Couldn't resolve proxy name
    R_DEFINE_ERROR_RESULT(_CURL_8006, 8006); // Couldn't resolve host name
    R_DEFINE_ERROR_RESULT(_CURL_8007, 8007); // Couldn't connect to server
    R_DEFINE_ERROR_RESULT(_CURL_8009, 8009); // Access denied to remote resource
    R_DEFINE_ERROR_RESULT(_CURL_8016, 8016); // Error in the HTTP2 framing layer
    R_DEFINE_ERROR_RESULT(_CURL_8018, 8018); // Transferred a partial file
    R_DEFINE_ERROR_RESULT(_CURL_8021, 8021); // Quote command returned error
    R_DEFINE_ERROR_RESULT(_CURL_8022, 8022); // HTTP response code said error
    R_DEFINE_ERROR_RESULT(_CURL_8023, 8023); // Failed writing received data to disk/application
    R_DEFINE_ERROR_RESULT(_CURL_8025, 8025); // Upload failed (at start/before it took off)
    R_DEFINE_ERROR_RESULT(_CURL_8026, 8026); // Failed to open/read local data from file/application
    R_DEFINE_ERROR_RESULT(_CURL_8027, 8027); // Out of memory
    R_DEFINE_ERROR_RESULT(_CURL_8028, 8028); // Timeout was reached
    R_DEFINE_ERROR_RESULT(_CURL_8033, 8033); // Requested range was not delivered by the server
    R_DEFINE_ERROR_RESULT(_CURL_8034, 8034); // Internal problem setting up the POST
    R_DEFINE_ERROR_RESULT(_CURL_8035, 8035); // SSL connect error
    R_DEFINE_ERROR_RESULT(_CURL_8036, 8036); // Couldn't resume download
    R_DEFINE_ERROR_RESULT(_CURL_8041, 8041); // A required function in the library was not found
    R_DEFINE_ERROR_RESULT(_CURL_8042, 8042); // Operation was aborted by an application callback
    R_DEFINE_ERROR_RESULT(_CURL_8043, 8043); // A libcurl function was given a bad argument
    R_DEFINE_ERROR_RESULT(_CURL_8045, 8045); // Failed binding local connection end
    R_DEFINE_ERROR_RESULT(_CURL_8047, 8047); // Number of redirects hit maximum amount
    R_DEFINE_ERROR_RESULT(_CURL_8048, 8048); // An unknown option was passed in to libcurl
    R_DEFINE_ERROR_RESULT(_CURL_8051, 8051); // Unknown error
    R_DEFINE_ERROR_RESULT(_CURL_8052, 8052); // Server returned nothing (no headers, no data)
    R_DEFINE_ERROR_RESULT(_CURL_8053, 8053); // SSL crypto engine not found
    R_DEFINE_ERROR_RESULT(_CURL_8054, 8054); // Can not set SSL crypto engine as default
    R_DEFINE_ERROR_RESULT(_CURL_8055, 8055); // Failed sending data to the peer
    R_DEFINE_ERROR_RESULT(_CURL_8056, 8056); // Failure when receiving data from the peer
    R_DEFINE_ERROR_RESULT(_CURL_8058, 8058); // Problem with the local SSL certificate
    R_DEFINE_ERROR_RESULT(_CURL_8059, 8059); // Couldn't use specified SSL cipher
    R_DEFINE_ERROR_RESULT(_CURL_8060, 8060); // SSL peer certificate or SSH remote key was not OK
    R_DEFINE_ERROR_RESULT(_CURL_8061, 8061); // Unrecognized or bad HTTP Content or Transfer-Encoding
    R_DEFINE_ERROR_RESULT(_CURL_8063, 8063); // Maximum file size exceeded
    R_DEFINE_ERROR_RESULT(_CURL_8064, 8064); // Requested SSL level failed
    R_DEFINE_ERROR_RESULT(_CURL_8065, 8065); // Send failed since rewinding of the data stream failed
    R_DEFINE_ERROR_RESULT(_CURL_8066, 8066); // Failed to initialise SSL crypto engine
    R_DEFINE_ERROR_RESULT(_CURL_8067, 8067); // Login denied
    R_DEFINE_ERROR_RESULT(_CURL_8075, 8075); // Conversion failed
    R_DEFINE_ERROR_RESULT(_CURL_8076, 8076); // Caller must register CURLOPT_CONV_ callback options
    R_DEFINE_ERROR_RESULT(_CURL_8077, 8077); // Problem with the SSL CA cert (path? access rights?)
    R_DEFINE_ERROR_RESULT(_CURL_8080, 8080); // Failed to shut down the SSL connection
    R_DEFINE_ERROR_RESULT(_CURL_8081, 8081); // Socket not ready for send/recv
    R_DEFINE_ERROR_RESULT(_CURL_8082, 8082); // Failed to load CRL file (path? access rights?, format?)
    R_DEFINE_ERROR_RESULT(_CURL_8083, 8083); // Issuer check against peer certificate failed
    R_DEFINE_ERROR_RESULT(_CURL_8088, 8088); // Chunk callback failed
    R_DEFINE_ERROR_RESULT(_CURL_8089, 8089); // The max connection limit is reached
    R_DEFINE_ERROR_RESULT(_CURL_8090, 8090); // SSL public key does not match pinned public key
    R_DEFINE_ERROR_RESULT(_CURL_8091, 8091); // SSL server certificate status verification FAILED
    R_DEFINE_ERROR_RESULT(_CURL_8092, 8092); // Stream error in the HTTP/2 framing layer
    R_DEFINE_ERROR_RESULT(_CURL_8093, 8093); // API function called from within callback
    R_DEFINE_ERROR_RESULT(_CURL_8094, 8094); // An authentication function returned an error
    R_DEFINE_ERROR_RESULT(_CURL_8095, 8095); // HTTP/3 error
    R_DEFINE_ERROR_RESULT(_CURL_8096, 8096); // QUIC connection error
    R_DEFINE_ERROR_RESULT(_CURL_8148, 8148); // Weird server reply

}
