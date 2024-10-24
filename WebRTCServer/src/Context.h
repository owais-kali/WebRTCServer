#pragma once
#include <mutex>
#include <map>
#include "rtc_base/ref_count.h"
#include "PeerConnectionObject.h"
#include "DummyAudioDevice.h"
#include "UnityVideoTrackSource.h"

namespace unity
{
    namespace webrtc
    {
        using namespace ::webrtc;

        class IGraphicsDevice;
        class ProfilerMarkerFactory;
        struct ContextDependencies
        {
            IGraphicsDevice* device;
            ProfilerMarkerFactory* profiler;
        };

        class Context;
        class MediaStreamObserver;
        class SetSessionDescriptionObserver;
        class ContextManager
        {
        public:
            static ContextManager* GetInstance();
            ~ContextManager();

            Context* GetContext(int uid) const;
            Context* CreateContext(int uid, ContextDependencies& dependencies);
            void DestroyContext(int uid);
            void SetCurContext(Context*);
            bool Exists(Context* context);
            using ContextPtr = std::unique_ptr<Context>;
            Context* curContext = nullptr;
            std::mutex mutex;

        private:
            std::map<int, ContextPtr> m_contexts;
            static std::unique_ptr<ContextManager> s_instance;
        };

        class Context {
        public:
            explicit Context(ContextDependencies& dependencies);
            ~Context();

            bool ExistsRefPtr(const rtc::RefCountInterface* ptr) const
            {
                return m_mapRefPtr.find(ptr) != m_mapRefPtr.end();
            }
            template<typename T>
            void AddRefPtr(rtc::scoped_refptr<T> refptr)
            {
                m_mapRefPtr.emplace(refptr.get(), refptr);
            }
            void AddRefPtr(rtc::RefCountInterface* ptr) { m_mapRefPtr.emplace(ptr, ptr); }

            template<typename T>
            void RemoveRefPtr(rtc::scoped_refptr<T>& refptr)
            {
                std::lock_guard<std::mutex> lock(mutex);
                m_mapRefPtr.erase(refptr.get());
            }
            template<typename T>
            void RemoveRefPtr(T* ptr)
            {
                std::lock_guard<std::mutex> lock(mutex);
                m_mapRefPtr.erase(ptr);
            }

            // Audio Source
            // rtc::scoped_refptr<AudioSourceInterface> CreateAudioSource();

            // Video Source
            rtc::scoped_refptr<UnityVideoTrackSource> CreateVideoSource();

            // MediaStreamTrack
            rtc::scoped_refptr<VideoTrackInterface>
                CreateVideoTrack(const std::string& label, webrtc::VideoTrackSourceInterface* source);
            rtc::scoped_refptr<AudioTrackInterface>
                CreateAudioTrack(const std::string& label, webrtc::AudioSourceInterface* source);

            // PeerConnection
            PeerConnectionObject* CreatePeerConnection(const PeerConnectionInterface::RTCConfiguration& config);
            void DeletePeerConnection(PeerConnectionObject* obj);

            // DataChannel
            DataChannelInterface*
                CreateDataChannel(PeerConnectionObject* obj, const char* label, const DataChannelInit& options);
            void AddDataChannel(rtc::scoped_refptr<DataChannelInterface> channel, PeerConnectionObject& pc);
            DataChannelObject* GetDataChannelObject(const DataChannelInterface* channel);
            void DeleteDataChannel(DataChannelInterface* channel);

            // mutex;
            std::mutex mutex;
        private:
            std::unique_ptr<rtc::Thread> m_workerThread;
            std::unique_ptr<rtc::Thread> m_signalingThread;
            // std::unique_ptr<TaskQueueFactory> m_taskQueueFactory;
            TaskQueueFactory* m_taskQueueFactory;
            rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> m_peerConnectionFactory;
            rtc::scoped_refptr<DummyAudioDevice> m_audioDevice;
            std::map<const PeerConnectionObject*, std::unique_ptr<PeerConnectionObject>> m_mapClients;

            std::map<const DataChannelInterface*, std::unique_ptr<DataChannelObject>> m_mapDataChannels;
            std::map<const rtc::RefCountInterface*, rtc::scoped_refptr<rtc::RefCountInterface>> m_mapRefPtr;

        };
    }
}