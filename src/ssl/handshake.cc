/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 *
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 *
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 *
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */
/* ====================================================================
 * Copyright (c) 1998-2002 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.openssl.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    openssl-core@openssl.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.openssl.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com). */
/* ====================================================================
 * Copyright 2002 Sun Microsystems, Inc. ALL RIGHTS RESERVED.
 * ECC cipher suite support in OpenSSL originally developed by
 * SUN MICROSYSTEMS, INC., and contributed to the OpenSSL project. */

#include <openssl/ssl.h>

#include <assert.h>

#include "../crypto/internal.h"
#include "internal.h"


namespace bssl {

SSL_HANDSHAKE::SSL_HANDSHAKE(SSL *ssl_arg)
    : ssl(ssl_arg),
      scts_requested(0),
      needs_psk_binder(0),
      received_hello_retry_request(0),
      received_custom_extension(0),
      accept_psk_mode(0),
      cert_request(0),
      certificate_status_expected(0),
      ocsp_stapling_requested(0),
      should_ack_sni(0),
      in_false_start(0),
      in_early_data(0),
      early_data_offered(0),
      can_early_read(0),
      can_early_write(0),
      next_proto_neg_seen(0),
      ticket_expected(0),
      extended_master_secret(0),
      pending_private_key_op(0) {
}

SSL_HANDSHAKE::~SSL_HANDSHAKE() {
  OPENSSL_cleanse(secret, sizeof(secret));
  OPENSSL_cleanse(early_traffic_secret, sizeof(early_traffic_secret));
  OPENSSL_cleanse(client_handshake_secret, sizeof(client_handshake_secret));
  OPENSSL_cleanse(server_handshake_secret, sizeof(server_handshake_secret));
  OPENSSL_cleanse(client_traffic_secret_0, sizeof(client_traffic_secret_0));
  OPENSSL_cleanse(server_traffic_secret_0, sizeof(server_traffic_secret_0));
  OPENSSL_free(cookie);
  OPENSSL_free(key_share_bytes);
  OPENSSL_free(ecdh_public_key);
  OPENSSL_free(peer_sigalgs);
  OPENSSL_free(peer_supported_group_list);
  OPENSSL_free(peer_key);
  OPENSSL_free(server_params);
  ssl->ctx->x509_method->hs_flush_cached_ca_names(this);
  OPENSSL_free(certificate_types);

  if (key_block != NULL) {
    OPENSSL_cleanse(key_block, key_block_len);
    OPENSSL_free(key_block);
  }
}

SSL_HANDSHAKE *ssl_handshake_new(SSL *ssl) {
  UniquePtr<SSL_HANDSHAKE> hs = MakeUnique<SSL_HANDSHAKE>(ssl);
  if (!hs ||
      !hs->transcript.Init()) {
    return nullptr;
  }
  return hs.release();
}

void ssl_handshake_free(SSL_HANDSHAKE *hs) { Delete(hs); }

int ssl_check_message_type(SSL *ssl, const SSLMessage &msg, int type) {
  if (msg.type != type) {
    ssl3_send_alert(ssl, SSL3_AL_FATAL, SSL_AD_UNEXPECTED_MESSAGE);
    OPENSSL_PUT_ERROR(SSL, SSL_R_UNEXPECTED_MESSAGE);
    ERR_add_error_dataf("got type %d, wanted type %d", msg.type, type);
    return 0;
  }

  return 1;
}

int ssl_add_message_cbb(SSL *ssl, CBB *cbb) {
  uint8_t *msg;
  size_t len;
  if (!ssl->method->finish_message(ssl, cbb, &msg, &len) ||
      !ssl->method->add_message(ssl, msg, len)) {
    return 0;
  }

  return 1;
}

size_t ssl_max_handshake_message_len(const SSL *ssl) {
  // kMaxMessageLen is the default maximum message size for handshakes which do
  // not accept peer certificate chains.
  static const size_t kMaxMessageLen = 16384;

  if (SSL_in_init(ssl)) {
    if ((!ssl->server || (ssl->verify_mode & SSL_VERIFY_PEER)) &&
        kMaxMessageLen < ssl->max_cert_list) {
      return ssl->max_cert_list;
    }
    return kMaxMessageLen;
  }

  if (ssl3_protocol_version(ssl) < TLS1_3_VERSION) {
    // In TLS 1.2 and below, the largest acceptable post-handshake message is
    // a HelloRequest.
    return 0;
  }

  if (ssl->server) {
    // The largest acceptable post-handshake message for a server is a
    // KeyUpdate. We will never initiate post-handshake auth.
    return 1;
  }

  // Clients must accept NewSessionTicket and CertificateRequest, so allow the
  // default size.
  return kMaxMessageLen;
}

bool ssl_hash_message(SSL_HANDSHAKE *hs, const SSLMessage &msg) {
  // V2ClientHello messages are pre-hashed.
  if (msg.is_v2_hello) {
    return true;
  }

  return hs->transcript.Update(CBS_data(&msg.raw), CBS_len(&msg.raw));
}

int ssl_parse_extensions(const CBS *cbs, uint8_t *out_alert,
                         const SSL_EXTENSION_TYPE *ext_types,
                         size_t num_ext_types, int ignore_unknown) {
  // Reset everything.
  for (size_t i = 0; i < num_ext_types; i++) {
    *ext_types[i].out_present = 0;
    CBS_init(ext_types[i].out_data, NULL, 0);
  }

  CBS copy = *cbs;
  while (CBS_len(&copy) != 0) {
    uint16_t type;
    CBS data;
    if (!CBS_get_u16(&copy, &type) ||
        !CBS_get_u16_length_prefixed(&copy, &data)) {
      OPENSSL_PUT_ERROR(SSL, SSL_R_PARSE_TLSEXT);
      *out_alert = SSL_AD_DECODE_ERROR;
      return 0;
    }

    const SSL_EXTENSION_TYPE *ext_type = NULL;
    for (size_t i = 0; i < num_ext_types; i++) {
      if (type == ext_types[i].type) {
        ext_type = &ext_types[i];
        break;
      }
    }

    if (ext_type == NULL) {
      if (ignore_unknown) {
        continue;
      }
      OPENSSL_PUT_ERROR(SSL, SSL_R_UNEXPECTED_EXTENSION);
      *out_alert = SSL_AD_UNSUPPORTED_EXTENSION;
      return 0;
    }

    // Duplicate ext_types are forbidden.
    if (*ext_type->out_present) {
      OPENSSL_PUT_ERROR(SSL, SSL_R_DUPLICATE_EXTENSION);
      *out_alert = SSL_AD_ILLEGAL_PARAMETER;
      return 0;
    }

    *ext_type->out_present = 1;
    *ext_type->out_data = data;
  }

  return 1;
}

static void set_crypto_buffer(CRYPTO_BUFFER **dest, CRYPTO_BUFFER *src) {
  // TODO(davidben): Remove this helper once |SSL_SESSION| can use |UniquePtr|
  // and |UniquePtr| has up_ref helpers.
  CRYPTO_BUFFER_free(*dest);
  *dest = src;
  if (src != nullptr) {
    CRYPTO_BUFFER_up_ref(src);
  }
}

enum ssl_verify_result_t ssl_verify_peer_cert(SSL_HANDSHAKE *hs) {
  SSL *const ssl = hs->ssl;
  const SSL_SESSION *prev_session = ssl->s3->established_session;
  if (prev_session != NULL) {
    // If renegotiating, the server must not change the server certificate. See
    // https://mitls.org/pages/attacks/3SHAKE. We never resume on renegotiation,
    // so this check is sufficient to ensure the reported peer certificate never
    // changes on renegotiation.
    assert(!ssl->server);
    if (sk_CRYPTO_BUFFER_num(prev_session->certs) !=
        sk_CRYPTO_BUFFER_num(hs->new_session->certs)) {
      OPENSSL_PUT_ERROR(SSL, SSL_R_SERVER_CERT_CHANGED);
      ssl3_send_alert(ssl, SSL3_AL_FATAL, SSL_AD_ILLEGAL_PARAMETER);
      return ssl_verify_invalid;
    }

    for (size_t i = 0; i < sk_CRYPTO_BUFFER_num(hs->new_session->certs); i++) {
      const CRYPTO_BUFFER *old_cert =
          sk_CRYPTO_BUFFER_value(prev_session->certs, i);
      const CRYPTO_BUFFER *new_cert =
          sk_CRYPTO_BUFFER_value(hs->new_session->certs, i);
      if (CRYPTO_BUFFER_len(old_cert) != CRYPTO_BUFFER_len(new_cert) ||
          OPENSSL_memcmp(CRYPTO_BUFFER_data(old_cert),
                         CRYPTO_BUFFER_data(new_cert),
                         CRYPTO_BUFFER_len(old_cert)) != 0) {
        OPENSSL_PUT_ERROR(SSL, SSL_R_SERVER_CERT_CHANGED);
        ssl3_send_alert(ssl, SSL3_AL_FATAL, SSL_AD_ILLEGAL_PARAMETER);
        return ssl_verify_invalid;
      }
    }

    // The certificate is identical, so we may skip re-verifying the
    // certificate. Since we only authenticated the previous one, copy other
    // authentication from the established session and ignore what was newly
    // received.
    set_crypto_buffer(&hs->new_session->ocsp_response,
                      prev_session->ocsp_response);
    set_crypto_buffer(&hs->new_session->signed_cert_timestamp_list,
                      prev_session->signed_cert_timestamp_list);
    hs->new_session->verify_result = prev_session->verify_result;
    return ssl_verify_ok;
  }

  uint8_t alert = SSL_AD_CERTIFICATE_UNKNOWN;
  enum ssl_verify_result_t ret;
  if (ssl->custom_verify_callback != nullptr) {
    ret = ssl->custom_verify_callback(ssl, &alert);
    switch (ret) {
      case ssl_verify_ok:
        hs->new_session->verify_result = X509_V_OK;
        break;
      case ssl_verify_invalid:
        hs->new_session->verify_result = X509_V_ERR_APPLICATION_VERIFICATION;
        break;
      case ssl_verify_retry:
        break;
    }
  } else {
    ret = ssl->ctx->x509_method->session_verify_cert_chain(
              hs->new_session.get(), ssl, &alert)
              ? ssl_verify_ok
              : ssl_verify_invalid;
  }

  if (ret == ssl_verify_invalid) {
    OPENSSL_PUT_ERROR(SSL, SSL_R_CERTIFICATE_VERIFY_FAILED);
    ssl3_send_alert(ssl, SSL3_AL_FATAL, alert);
  }

  return ret;
}

uint16_t ssl_get_grease_value(const SSL *ssl, enum ssl_grease_index_t index) {
  // Use the client_random or server_random for entropy. This both avoids
  // calling |RAND_bytes| on a single byte repeatedly and ensures the values are
  // deterministic. This allows the same ClientHello be sent twice for a
  // HelloRetryRequest or the same group be advertised in both supported_groups
  // and key_shares.
  uint16_t ret = ssl->server ? ssl->s3->server_random[index]
                             : ssl->s3->client_random[index];
  // The first four bytes of server_random are a timestamp prior to TLS 1.3, but
  // servers have no fields to GREASE until TLS 1.3.
  assert(!ssl->server || ssl3_protocol_version(ssl) >= TLS1_3_VERSION);
  // This generates a random value of the form 0xωaωa, for all 0 ≤ ω < 16.
  ret = (ret & 0xf0) | 0x0a;
  ret |= ret << 8;
  return ret;
}

enum ssl_hs_wait_t ssl_get_finished(SSL_HANDSHAKE *hs) {
  SSL *const ssl = hs->ssl;
  SSLMessage msg;
  if (!ssl->method->get_message(ssl, &msg)) {
    return ssl_hs_read_message;
  }

  if (!ssl_check_message_type(ssl, msg, SSL3_MT_FINISHED)) {
    return ssl_hs_error;
  }

  // Snapshot the finished hash before incorporating the new message.
  uint8_t finished[EVP_MAX_MD_SIZE];
  size_t finished_len;
  if (!hs->transcript.GetFinishedMAC(finished, &finished_len,
                                     SSL_get_session(ssl), !ssl->server,
                                     ssl3_protocol_version(ssl)) ||
      !ssl_hash_message(hs, msg)) {
    return ssl_hs_error;
  }

  int finished_ok = CBS_mem_equal(&msg.body, finished, finished_len);
#if defined(BORINGSSL_UNSAFE_FUZZER_MODE)
  finished_ok = 1;
#endif
  if (!finished_ok) {
    ssl3_send_alert(ssl, SSL3_AL_FATAL, SSL_AD_DECRYPT_ERROR);
    OPENSSL_PUT_ERROR(SSL, SSL_R_DIGEST_CHECK_FAILED);
    return ssl_hs_error;
  }

  // Copy the Finished so we can use it for renegotiation checks.
  if (ssl->version != SSL3_VERSION) {
    if (finished_len > sizeof(ssl->s3->previous_client_finished) ||
        finished_len > sizeof(ssl->s3->previous_server_finished)) {
      OPENSSL_PUT_ERROR(SSL, ERR_R_INTERNAL_ERROR);
      return ssl_hs_error;
    }

    if (ssl->server) {
      OPENSSL_memcpy(ssl->s3->previous_client_finished, finished, finished_len);
      ssl->s3->previous_client_finished_len = finished_len;
    } else {
      OPENSSL_memcpy(ssl->s3->previous_server_finished, finished, finished_len);
      ssl->s3->previous_server_finished_len = finished_len;
    }
  }

  ssl->method->next_message(ssl);
  return ssl_hs_ok;
}

int ssl_run_handshake(SSL_HANDSHAKE *hs, int *out_early_return) {
  SSL *const ssl = hs->ssl;
  for (;;) {
    // Resolve the operation the handshake was waiting on.
    switch (hs->wait) {
      case ssl_hs_error:
        OPENSSL_PUT_ERROR(SSL, SSL_R_SSL_HANDSHAKE_FAILURE);
        return -1;

      case ssl_hs_flush: {
        int ret = ssl->method->flush_flight(ssl);
        if (ret <= 0) {
          return ret;
        }
        break;
      }

      case ssl_hs_read_server_hello:
      case ssl_hs_read_message: {
        int ret = ssl->method->read_message(ssl);
        if (ret <= 0) {
          uint32_t err = ERR_peek_error();
          if (hs->wait == ssl_hs_read_server_hello &&
              ERR_GET_LIB(err) == ERR_LIB_SSL &&
              ERR_GET_REASON(err) == SSL_R_SSLV3_ALERT_HANDSHAKE_FAILURE) {
            // Add a dedicated error code to the queue for a handshake_failure
            // alert in response to ClientHello. This matches NSS's client
            // behavior and gives a better error on a (probable) failure to
            // negotiate initial parameters. Note: this error code comes after
            // the original one.
            //
            // See https://crbug.com/446505.
            OPENSSL_PUT_ERROR(SSL, SSL_R_HANDSHAKE_FAILURE_ON_CLIENT_HELLO);
          }
          return ret;
        }
        break;
      }

      case ssl_hs_read_change_cipher_spec: {
        int ret = ssl->method->read_change_cipher_spec(ssl);
        if (ret <= 0) {
          return ret;
        }
        break;
      }

      case ssl_hs_read_end_of_early_data: {
        if (ssl->s3->hs->can_early_read) {
          // While we are processing early data, the handshake returns early.
          *out_early_return = 1;
          return 1;
        }
        hs->wait = ssl_hs_ok;
        break;
      }

      case ssl_hs_certificate_selection_pending:
        ssl->rwstate = SSL_CERTIFICATE_SELECTION_PENDING;
        hs->wait = ssl_hs_ok;
        return -1;

      case ssl_hs_x509_lookup:
        ssl->rwstate = SSL_X509_LOOKUP;
        hs->wait = ssl_hs_ok;
        return -1;

      case ssl_hs_channel_id_lookup:
        ssl->rwstate = SSL_CHANNEL_ID_LOOKUP;
        hs->wait = ssl_hs_ok;
        return -1;

      case ssl_hs_private_key_operation:
        ssl->rwstate = SSL_PRIVATE_KEY_OPERATION;
        hs->wait = ssl_hs_ok;
        return -1;

      case ssl_hs_pending_session:
        ssl->rwstate = SSL_PENDING_SESSION;
        hs->wait = ssl_hs_ok;
        return -1;

      case ssl_hs_pending_ticket:
        ssl->rwstate = SSL_PENDING_TICKET;
        hs->wait = ssl_hs_ok;
        return -1;

      case ssl_hs_certificate_verify:
        ssl->rwstate = SSL_CERTIFICATE_VERIFY;
        hs->wait = ssl_hs_ok;
        return -1;

      case ssl_hs_early_data_rejected:
        ssl->rwstate = SSL_EARLY_DATA_REJECTED;
        // Cause |SSL_write| to start failing immediately.
        hs->can_early_write = 0;
        return -1;

      case ssl_hs_early_return:
        *out_early_return = 1;
        hs->wait = ssl_hs_ok;
        return 1;

      case ssl_hs_ok:
        break;
    }

    // Run the state machine again.
    hs->wait = ssl->do_handshake(hs);
    if (hs->wait == ssl_hs_error) {
      // Don't loop around to avoid a stray |SSL_R_SSL_HANDSHAKE_FAILURE| the
      // first time around.
      return -1;
    }
    if (hs->wait == ssl_hs_ok) {
      // The handshake has completed.
      return 1;
    }

    // Otherwise, loop to the beginning and resolve what was blocking the
    // handshake.
  }
}

}  // namespace bssl