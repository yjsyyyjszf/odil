/*************************************************************************
 * dcmtkpp - Copyright (C) Universite de Strasbourg
 * Distributed under the terms of the CeCILL-B license, as published by
 * the CEA-CNRS-INRIA. Refer to the LICENSE file or to
 * http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
 * for details.
 ************************************************************************/

#ifndef _a52696bc_5c6e_402d_a343_6cb085eb0138
#define _a52696bc_5c6e_402d_a343_6cb085eb0138

#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include "dcmtkpp/AssociationAcceptor.h"
#include "dcmtkpp/AssociationParameters.h"
#include "dcmtkpp/dul/StateMachine.h"
#include "dcmtkpp/message/Message.h"

namespace dcmtkpp
{

/**
 * @brief Association.
 */
class Association
{
public:
    /// @brief Association result (ITU-T X.227, PS 3.8, 7.1.1.7 and PS 3.8, 9.3.4).
    enum Result
    {
        Accepted=0,
        RejectedPermanent=1,
        RejectedTransient=2,
    };

    /// @brief Source of association result (PS 3.8, 7.1.1.8 and PS 3.8, 9.3.4).
    enum ResultSource
    {
        ULServiceUser=1,
        ULServiceProviderACSERelatedFunction=2,
        ULServiceProvderPresentationRelatedFunction=3,
    };

    // PS 3.8, 7.1.1.9, and PS 3.8, 9.3.4, and ITU-T X.227 (UL service-user,
    // UL service-provider ACSE-related function), ITU-T X.226 (UL
    // service-provider presentation-related function)
    /// @brief Diagnostic of association result
    enum Diagnostic
    {
        // UL service-user
        NoReasonGiven=1,
        ApplicationContextNameNotSupported=2,
        CallingAETitleNotRecognized=3,
        CallingAPInvocationIdentifierNotRecognized=4,
        CallingAEQualifierNotRecognized=5,
        CallingAEInvocationIdentifierNotRecognized=6,
        CalledAETitleNotRecognized=7,
        CalledAPInvocationIdentifierNotRecognized=8,
        CalledAEQualifierNotRecognized=9,
        CalledAEInvocationIdentifierNotRecognized=10,

        // UL service-provider, ACSE-related function
        NoCommonULVersion=2,

        // UL service-provider, presentation-related function
        TemporaryCongestion=1,
        LocalLimitExceeded=2,
        CalledPresentationAddressUnknown=3,
        PresentationProtocolVersionNotSupported=4,
        NoPresentationServiceAccessPointAvailable=7,
    };

    typedef dul::StateMachine::duration_type duration_type;

    /// @brief Create a default, un-associated, association.
    Association();

    /// @brief Create an un-associated association.
    Association(Association const & other);

    /// @brief Destroy the association, release it if necessary.
    ~Association();

    dul::Transport & get_transport();

    /// @brief Assing an un-associated association; it remains un-associated.
    Association & operator=(Association const & other);

    /// @name Peer
    /// @{

    /// @brief Return the host name of the peer. Defaults to "".
    std::string const & get_peer_host() const;
    /// @brief Set the host name of the peer.
    void set_peer_host(std::string const & host);

    /// @brief Return the port of the peer. Defaults to 104.
    uint16_t get_peer_port() const;
    /// @brief Set the port of the peer.
    void set_peer_port(uint16_t port);

    /// @}

    /// @brief Return the association parameters.
    AssociationParameters const & get_parameters() const;

    /// @brief Return the association parameters.
    AssociationParameters & update_parameters();

    /// @brief Set the association parameters, throw an exception when associated.
    void set_parameters(AssociationParameters const & value);

    /// @name Timeouts
    /// @{

    /// @brief Return the TCP timeout, default to infinity.
    duration_type get_tcp_timeout() const;

    /// @brief Set the timeout.
    void set_tcp_timeout(duration_type const & duration);

    /// @brief Return the DIMSE timeout, default to 30s.
    duration_type get_message_timeout() const;

    /// @brief Set the DIMSE timeout.
    void set_message_timeout(duration_type const & duration);

    /// @}

    /// @name Association
    /// @{

    /// @brief Test whether the object is currently associated to its peer.
    bool is_associated() const;

    /// @brief Request an association with the peer.
    void associate();

    /// @brief Receive an association from a peer.
    void receive_association(
        boost::asio::ip::tcp const & protocol, unsigned short port,
        AssociationAcceptor acceptor=default_association_acceptor);

    /// @brief Reject the received association request.
    void reject(Result result, ResultSource result_source, Diagnostic diagnostic);

    /// @brief Gracefully release the association. Throws an exception if not associated.
    void release();
    /// @brief Forcefully release the association. Throws an exception if not associated.
    void abort(int source, int reason);

    /// @}

    /// @name DIMSE messages sending and reception.
    /// @{

    /// @brief Receive a generic DIMSE message.
    message::Message receive_message();

    /// @brief Send a DIMSE message.
    void send_message(
        message::Message const & message, std::string const & abstract_syntax);

    /// @brief Return the next available message id.
    uint16_t next_message_id();

    /// @}

private:
    dul::StateMachine _state_machine;

    std::string _peer_host;
    uint16_t _peer_port;

    AssociationParameters _association_parameters;

    std::map<std::string, std::pair<uint8_t, std::string>>
        _transfer_syntaxes_by_abstract_syntax;
    std::map<uint8_t, std::string> _transfer_syntaxes_by_id;

    uint16_t _next_message_id;
};

}

#endif // _a52696bc_5c6e_402d_a343_6cb085eb0138
