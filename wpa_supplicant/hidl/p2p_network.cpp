/*
 * hidl interface for wpa_supplicant daemon
 * Copyright (c) 2004-2016, Jouni Malinen <j@w1.fi>
 * Copyright (c) 2004-2016, Roshan Pius <rpius@google.com>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "hidl_manager.h"
#include "hidl_return_util.h"
#include "p2p_network.h"

namespace android {
namespace hardware {
namespace wifi {
namespace supplicant {
namespace V1_0 {
namespace implementation {
using hidl_return_util::validateAndCall;

P2pNetwork::P2pNetwork(
    struct wpa_global *wpa_global, const char ifname[], int network_id)
    : wpa_global_(wpa_global),
      ifname_(ifname),
      network_id_(network_id),
      is_valid_(true)
{
}

void P2pNetwork::invalidate() { is_valid_ = false; }
bool P2pNetwork::isValid()
{
	return (is_valid_ && (retrieveNetworkPtr() != nullptr));
}

Return<void> P2pNetwork::getId(getId_cb _hidl_cb)
{
	return validateAndCall(
	    this, SupplicantStatusCode::FAILURE_NETWORK_INVALID,
	    &P2pNetwork::getIdInternal, _hidl_cb);
}

Return<void> P2pNetwork::getInterfaceName(getInterfaceName_cb _hidl_cb)
{
	return validateAndCall(
	    this, SupplicantStatusCode::FAILURE_NETWORK_INVALID,
	    &P2pNetwork::getInterfaceNameInternal, _hidl_cb);
}

Return<void> P2pNetwork::getType(getType_cb _hidl_cb)
{
	return validateAndCall(
	    this, SupplicantStatusCode::FAILURE_NETWORK_INVALID,
	    &P2pNetwork::getTypeInternal, _hidl_cb);
}

Return<void> P2pNetwork::registerCallback(
    const sp<ISupplicantP2pNetworkCallback> &callback,
    registerCallback_cb _hidl_cb)
{
	return validateAndCall(
	    this, SupplicantStatusCode::FAILURE_NETWORK_INVALID,
	    &P2pNetwork::registerCallbackInternal, _hidl_cb, callback);
}

std::pair<SupplicantStatus, uint32_t> P2pNetwork::getIdInternal()
{
	return {{SupplicantStatusCode::SUCCESS, ""}, network_id_};
}

std::pair<SupplicantStatus, std::string> P2pNetwork::getInterfaceNameInternal()
{
	return {{SupplicantStatusCode::SUCCESS, ""}, ifname_};
}

std::pair<SupplicantStatus, IfaceType> P2pNetwork::getTypeInternal()
{
	return {{SupplicantStatusCode::SUCCESS, ""}, IfaceType::P2P};
}

SupplicantStatus P2pNetwork::registerCallbackInternal(
    const sp<ISupplicantP2pNetworkCallback> &callback)
{
	HidlManager *hidl_manager = HidlManager::getInstance();
	if (!hidl_manager ||
	    hidl_manager->addP2pNetworkCallbackHidlObject(
		ifname_, network_id_, callback)) {
		return {SupplicantStatusCode::FAILURE_UNKNOWN, ""};
	}
	return {SupplicantStatusCode::SUCCESS, ""};
}

/**
 * Retrieve the underlying |wpa_ssid| struct pointer for
 * this network.
 * If the underlying network is removed or the interface
 * this network belong to is removed, all RPC method calls
 * on this object will return failure.
 */
struct wpa_ssid *P2pNetwork::retrieveNetworkPtr()
{
	wpa_supplicant *wpa_s = retrieveIfacePtr();
	if (!wpa_s)
		return nullptr;
	return wpa_config_get_network(wpa_s->conf, network_id_);
}

/**
 * Retrieve the underlying |wpa_supplicant| struct
 * pointer for this network.
 */
struct wpa_supplicant *P2pNetwork::retrieveIfacePtr()
{
	return wpa_supplicant_get_iface(
	    (struct wpa_global *)wpa_global_, ifname_.c_str());
}
}  // namespace implementation
}  // namespace V1_0
}  // namespace wifi
}  // namespace supplicant
}  // namespace hardware
}  // namespace android
