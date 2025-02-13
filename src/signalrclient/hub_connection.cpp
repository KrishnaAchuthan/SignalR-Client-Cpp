// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

#include "stdafx.h"
#include "signalrclient/hub_connection.h"
#include "hub_connection_impl.h"
#include "signalrclient/signalr_exception.h"
#include "signalrclient/http_client.h"
#include "signalrclient/websocket_client.h"
#include "hub_protocol.h"

namespace signalr
{
    hub_connection::hub_connection(const std::string& url, std::unique_ptr<hub_protocol>&& hub_protocol,
        trace_level trace_level, std::shared_ptr<log_writer> log_writer, std::function<std::shared_ptr<http_client>(const signalr_client_config&)> http_client_factory,
        std::function<std::shared_ptr<websocket_client>(const signalr_client_config&)> websocket_factory, const bool skip_negotiation)
        : m_pImpl(hub_connection_impl::create(url, std::move(hub_protocol), trace_level, log_writer, http_client_factory, websocket_factory, skip_negotiation))
    {}

    hub_connection::hub_connection(hub_connection&& rhs) noexcept
        : m_pImpl(std::move(rhs.m_pImpl))
    {}

    hub_connection& hub_connection::operator=(hub_connection&& rhs) noexcept
    {
        m_pImpl = std::move(rhs.m_pImpl);

        return *this;
    }

    // Do NOT remove this destructor. Letting the compiler generate and inline the default dtor may lead to
    // undefined behavior since we are using an incomplete type. More details here:  http://herbsutter.com/gotw/_100/
    hub_connection::~hub_connection() = default;

    void hub_connection::start(std::function<void(std::exception_ptr)> callback) noexcept
    {
        if (!m_pImpl)
        {
            callback(std::make_exception_ptr(signalr_exception("start() cannot be called on destructed hub_connection instance")));
        }

        m_pImpl->start(callback);
    }

    void hub_connection::stop(std::function<void(std::exception_ptr)> callback) noexcept
    {
        if (!m_pImpl)
        {
            callback(std::make_exception_ptr(signalr_exception("stop() cannot be called on destructed hub_connection instance")));
        }

        m_pImpl->stop(callback);
    }

    void hub_connection::on(const std::string& event_name, const method_invoked_handler& handler)
    {
        if (!m_pImpl)
        {
            throw signalr_exception("on() cannot be called on destructed hub_connection instance");
        }

        return m_pImpl->on(event_name, handler);
    }

    void hub_connection::invoke(const std::string& method_name, const std::vector<signalr::value>& arguments, std::function<void(const signalr::value&, std::exception_ptr)> callback) noexcept
    {
        if (!m_pImpl)
        {
            callback(signalr::value(), std::make_exception_ptr(signalr_exception("invoke() cannot be called on destructed hub_connection instance")));
            return;
        }

        return m_pImpl->invoke(method_name, arguments, callback);
    }

    void hub_connection::send(const std::string& method_name, const std::vector<signalr::value>& arguments, std::function<void(std::exception_ptr)> callback) noexcept
    {
        if (!m_pImpl)
        {
            callback(std::make_exception_ptr(signalr_exception("send() cannot be called on destructed hub_connection instance")));
            return;
        }

        m_pImpl->send(method_name, arguments, callback);
    }

    connection_state hub_connection::get_connection_state() const
    {
        if (!m_pImpl)
        {
            throw signalr_exception("get_connection_state() cannot be called on destructed hub_connection instance");
        }

        return m_pImpl->get_connection_state();
    }

    std::string hub_connection::get_connection_id() const
    {
        if (!m_pImpl)
        {
            throw signalr_exception("get_connection_id() cannot be called on destructed hub_connection instance");
        }

        return m_pImpl->get_connection_id();
    }

    void hub_connection::set_disconnected(const std::function<void(std::exception_ptr)>& disconnected_callback)
    {
        if (!m_pImpl)
        {
            throw signalr_exception("set_disconnected() cannot be called on destructed hub_connection instance");
        }

        m_pImpl->set_disconnected(disconnected_callback);
    }

    void hub_connection::set_client_config(const signalr_client_config& config)
    {
        if (!m_pImpl)
        {
            throw signalr_exception("set_client_config() cannot be called on destructed hub_connection instance");
        }

        m_pImpl->set_client_config(config);
    }
}
