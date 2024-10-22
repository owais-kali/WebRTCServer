#include "PeerConnectionObject.h"
#include "Logger.h"

namespace unity
{
	namespace webrtc
	{
		webrtc::SdpType ConvertSdpType(RTCSdpType type)
		{
			switch (type)
			{
			case RTCSdpType::Offer:
				return webrtc::SdpType::kOffer;
			case RTCSdpType::PrAnswer:
				return webrtc::SdpType::kPrAnswer;
			case RTCSdpType::Answer:
				return webrtc::SdpType::kAnswer;
			case RTCSdpType::Rollback:
				return webrtc::SdpType::kRollback;
			}
			// throw std::invalid_argument("Unknown RTCSdpType");
		}

		RTCSdpType ConvertSdpType(webrtc::SdpType type)
		{
			switch (type)
			{
			case webrtc::SdpType::kOffer:
				return RTCSdpType::Offer;
			case webrtc::SdpType::kPrAnswer:
				return RTCSdpType::PrAnswer;
			case webrtc::SdpType::kAnswer:
				return RTCSdpType::Answer;
			case webrtc::SdpType::kRollback:
				return RTCSdpType::Rollback;
			}
			// throw std::invalid_argument("Unknown SdpType");
		}

		PeerConnectionObject::PeerConnectionObject(Context& context)
			:context(context)
		{
			JLogPrint(LoggingSeverity::LS_INFO, "PeerConnectionObject Created!");
		}

		PeerConnectionObject::~PeerConnectionObject()
		{
			JLogPrint(LoggingSeverity::LS_INFO, "PeerConnectionObject Destroyed!");
			if (connection == nullptr)
			{
				return;
			}
			auto senders = connection->GetSenders();
			for (const auto& sender : senders)
			{
				// ignore error.
				connection->RemoveTrackOrError(sender);
			}

			const auto state = connection->peer_connection_state();
			if (state != webrtc::PeerConnectionInterface::PeerConnectionState::kClosed)
			{
				connection->Close();
			}
			connection = nullptr;
		}

		void PeerConnectionObject::Close()
		{
			if (connection != nullptr &&
				connection->peer_connection_state() != webrtc::PeerConnectionInterface::PeerConnectionState::kClosed)
			{
				// Cleanup delegates/callbacks
				onCreateSDSuccess = nullptr;
				onCreateSDFailure = nullptr;
				onLocalSdpReady = nullptr;
				onIceCandidate = nullptr;
				onIceConnectionChange = nullptr;
				onDataChannel = nullptr;
				onRenegotiationNeeded = nullptr;
				onTrack = nullptr;

				connection->Close();
			}
		}

		RTCErrorType PeerConnectionObject::SetLocalDescription(const RTCSessionDescription& desc, rtc::scoped_refptr<SetLocalDescriptionObserverInterface> observer, std::string& error)
		{
			SdpParseError error_;
			std::unique_ptr<SessionDescriptionInterface> _desc =
				CreateSessionDescription(ConvertSdpType(desc.type), desc.sdp, &error_);
			if (!_desc)
			{
				error = error_.description;
				return RTCErrorType::SYNTAX_ERROR;
			}
			connection->SetLocalDescription(std::move(_desc), observer);
			return RTCErrorType::NONE;
		}

		RTCErrorType PeerConnectionObject::SetLocalDescriptionWithoutDescription(rtc::scoped_refptr<SetLocalDescriptionObserverInterface> observer, std::string& error)
		{
			connection->SetLocalDescription(observer);
			return RTCErrorType::NONE;
		}

		RTCErrorType PeerConnectionObject::SetRemoteDescription(const RTCSessionDescription& desc, rtc::scoped_refptr<SetRemoteDescriptionObserverInterface> observer, std::string& error)
		{
			SdpParseError error_;
			std::unique_ptr<SessionDescriptionInterface> _desc =
				CreateSessionDescription(ConvertSdpType(desc.type), desc.sdp, &error_);
			if (!_desc)
			{
				error = error_.description;
				return RTCErrorType::SYNTAX_ERROR;
			}
			connection->SetRemoteDescription(std::move(_desc), observer);
			return RTCErrorType::NONE;
		}

		bool PeerConnectionObject::GetSessionDescription(const SessionDescriptionInterface* sdp, RTCSessionDescription& desc) const
		{
			return false;
		}

		RTCErrorType PeerConnectionObject::SetConfiguration(const std::string& config)
		{
			return RTCErrorType();
		}

		std::string PeerConnectionObject::GetConfiguration() const
		{
			return std::string();
		}

		void PeerConnectionObject::CreateOffer(const RTCOfferAnswerOptions& options, CreateSessionDescriptionObserver* observer)
		{
			webrtc::PeerConnectionInterface::RTCOfferAnswerOptions _options;
			_options.ice_restart = options.iceRestart;
			_options.voice_activity_detection = options.voiceActivityDetection;
			connection->CreateOffer(observer, _options);
		}

		void PeerConnectionObject::CreateAnswer(const RTCOfferAnswerOptions& options, CreateSessionDescriptionObserver* observer)
		{
			webrtc::PeerConnectionInterface::RTCOfferAnswerOptions _options;
			_options.ice_restart = options.iceRestart;
			_options.voice_activity_detection = options.voiceActivityDetection;
			connection->CreateAnswer(observer, _options);
		}
		void PeerConnectionObject::OnSignalingChange(PeerConnectionInterface::SignalingState new_state)
		{
			
		}
		void PeerConnectionObject::OnAddStream(rtc::scoped_refptr<MediaStreamInterface> stream)
		{
			
		}
		void PeerConnectionObject::OnRemoveStream(rtc::scoped_refptr<MediaStreamInterface> stream)
		{
		}
		void PeerConnectionObject::OnDataChannel(rtc::scoped_refptr<DataChannelInterface> data_channel)
		{
			
		}
		void PeerConnectionObject::OnRenegotiationNeeded()
		{
			JLogPrint(LoggingSeverity::LS_INFO, "OnRenegotiationNeeded");
		}
		void PeerConnectionObject::OnIceConnectionChange(PeerConnectionInterface::IceConnectionState new_state)
		{
			JLogPrint(LoggingSeverity::LS_INFO, "OnIceConnectionChange");
		}
		void PeerConnectionObject::OnConnectionChange(PeerConnectionInterface::PeerConnectionState new_state)
		{
			JLogPrint(LoggingSeverity::LS_INFO, "OnConnectionChange: state: %d", new_state);
			if (onConnectionStateChange)
				onConnectionStateChange(this,new_state);
		}
		void PeerConnectionObject::OnIceGatheringChange(PeerConnectionInterface::IceGatheringState new_state)
		{
		}
		void PeerConnectionObject::OnIceCandidate(const IceCandidateInterface* candidate)
		{
			std::string out;
			if (!candidate->ToString(&out))
			{
				DebugError("Can't make string form of sdp.");
			}
			if (onIceCandidate != nullptr)
			{
				onIceCandidate(this, out.c_str(), candidate->sdp_mid().c_str(), candidate->sdp_mline_index());
			}
		}
		void PeerConnectionObject::OnTrack(rtc::scoped_refptr<RtpTransceiverInterface> transceiver)
		{
		}
		void PeerConnectionObject::OnRemoveTrack(rtc::scoped_refptr<RtpReceiverInterface> receiver)
		{
		}
	}
}