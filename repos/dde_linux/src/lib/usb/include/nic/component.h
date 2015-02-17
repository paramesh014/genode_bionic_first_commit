/*
 * \brief  Block-session implementation for network devices
 * \author Sebastian Sumpf
 * \date   2012-07-05
 */

/*
 * Copyright (C) 2012-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _NIC__COMPONENT_H_
#define _NIC__COMPONENT_H_

#include <root/component.h>
#include <nic/packet_allocator.h>
#include <nic_session/rpc_object.h>
#include <timer_session/connection.h>
#include <signal/dispatch.h>

#define BENCH 0

namespace Nic {

	using namespace Genode;
	class Session_component;

#if BENCH
	struct Counter : public Genode::Thread<8192>
	{
		char const *prefix;
		int         cnt;
		int         burst;
		size_t      size;

		void entry()
		{
			Timer::Connection _timer;
			int interval = 5;
			while(1) {
				_timer.msleep(interval * 1000);
				PDBG("%s: Packets %d/s (in %d burst packets)  bytes/s: %d",
				     prefix, cnt / interval, burst / interval,  size / interval);
				cnt = 0;
				size = 0;
				burst = 0;
			}
		}

		void inc(size_t s) { cnt++; size += s; }
		void inc_burst() { burst++; }

		Counter(char const *prefix)
		: Thread("counter"), prefix(prefix), cnt(0), burst(0), size(0)
		{ start(); }
	};
#else
	struct Counter
	{
		Counter(char const *) { };
		void inc(size_t) { }
		void inc_burst() { }
	};
#endif


	struct Device : ::Device
	{
		Session_component *_session;

		/**
		 * Transmit data to driver
		 */
		virtual bool tx(addr_t virt, size_t size) = 0;

		/**
		 * Return mac address of device
		 */
		virtual Mac_address mac_address() = 0;

		/**
		 * Set session belonging to this driver
		 */
		void session(Session_component *s) { _session = s; }

		/**
		 * Check for session
		 */
		bool session() { return _session != 0; }

		/**
		 * Alloc an SKB
		 */
		virtual sk_buff *alloc_skb() = 0;

		/**
		 * Submit SKB to device
		 */
		virtual void tx_skb(sk_buff *skb) = 0;

		/**
		 * Setup SKB with 'data' of 'size', return 'false' if SKB is longer than
		 * 'end'.
		 */
		virtual bool skb_fill(struct sk_buff *skb, unsigned char *data, Genode::size_t size, unsigned char *end) = 0;

		/**
		 * Call driver fixup function on SKB
		 */
		virtual void tx_fixup(struct sk_buff *skb) = 0;

		/**
		 * Return true if device supports burst operations
		 */
		virtual bool burst() = 0;

		Device() : _session(0) { }
	};


	class Session_component : public Nic::Packet_allocator,
	                          public Packet_session_component<Session_rpc_object>
	{
		private:

			Device           *_device;    /* device this session is using */
			Tx::Sink         *_tx_sink;   /* client packet sink */
			bool              _tx_alloc;  /* get next packet from client or use _tx_packet */
			Packet_descriptor _tx_packet; /* saved packet in case of driver errors */

			void _send_packet_avail_signal() {
				Signal_transmitter(_tx.sigh_packet_avail()).submit(); }

		protected:

			void _process_packets(unsigned)
			{
				static sk_buff work_skb; /* dummy skb for fixup calls */
				static Counter counter("TX");

				int tx_cnt         = 0;
				unsigned size      = 0;
				sk_buff *skb       = 0;
				unsigned char *ptr = 0;

				/* submit received packets to lower layer */
				while (_tx_sink->packet_avail())
				{
					Packet_descriptor packet = _tx_alloc ? _tx_sink->get_packet() : _tx_packet;
					addr_t virt = (addr_t)_tx_sink->packet_content(packet);

					if (_device->burst()) {
						if (!ptr || !_device->skb_fill(&work_skb, ptr, packet.size(), skb->end)) {

							/* submit batch to device */
							if (ptr) {
								_device->tx_skb(skb);
								tx_cnt++;
								counter.inc_burst();
							}

							/* alloc new SKB */
							skb = _device->alloc_skb();

							if (!skb) {
								_send_packet_avail_signal();
								_tx_alloc = false;
								_tx_packet = packet;
								return;
							}

							_tx_alloc = true;

							ptr = skb->data;
							work_skb.data = 0;
							_device->skb_fill(&work_skb, ptr, packet.size(), skb->end);
						}

						/* copy packet to current data pos */
						Genode::memcpy(work_skb.data, (void *)virt, packet.size());
						/* call fixup on dummy SKB */
						_device->tx_fixup(&work_skb);
						/* advance to next slot */
						ptr       = work_skb.end;
						skb->len += work_skb.truesize;
					} else {

						/* send to driver */
						if (!(_device->tx(virt, packet.size()))) {
							_send_packet_avail_signal();
							_tx_alloc  = false;
							_tx_packet = packet;
							return;
						}

						_tx_alloc = true;

						tx_cnt++;
					}

					counter.inc(packet.size());

					/* acknowledge to client */
					_tx_sink->acknowledge_packet(packet);

					/* it's cooperative scheduling - be nice */
					if (tx_cnt == 20)
						break;
				}

				/* sumbit last skb */
				if (skb) {
					_device->tx_skb(skb);
					counter.inc_burst();
				}

				/* for large TCP/s check RX immediately */
				Irq::check_irq();

				/* release acknowledged packets */
				_rx_ack(false);

				if (_tx_sink->packet_avail())
					_send_packet_avail_signal();
			}

			void _rx_ack(bool block = true)
			{
				while (_rx.source()->ack_avail() || block)
				{
					
					Packet_descriptor packet = _rx.source()->get_acked_packet();

					/* free packet buffer */
					_rx.source()->release_packet(packet);
					block = false;
				}
			}

		public:

			/**
			 * Constructor
			 */
			Session_component(Dataspace_capability  tx_ds,
			                  Dataspace_capability  rx_ds,
			                  Server::Entrypoint   &ep,
			                  ::Device *device)
			:
				Nic::Packet_allocator(Genode::env()->heap()),
				Packet_session_component(tx_ds, rx_ds, this, ep),
				_device(static_cast<Device *>(device)),
				_tx_sink(Session_rpc_object::_tx.sink()),
				_tx_alloc(true)
				{ _device->session(this); }

			Mac_address mac_address() { return _device->mac_address(); }

			/**
			 * Send packet to client (called form driver)
			 */
			void rx(addr_t virt, size_t size)
			{
				static Counter counter("RX");

				while (true) {
					try {
						Packet_descriptor p =_rx.source()->alloc_packet(size);
						Genode::memcpy(_rx.source()->packet_content(p), (void*)virt, size);
						_rx.source()->submit_packet(p);
						counter.inc(size);
					} catch (...) {
						/* ack or block */
						_rx_ack();
						continue;
					}
					break;
				}

				_rx_ack(false);
			}
	};

	/*
	 * Shortcut for single-client root component
	 */
	typedef Root_component<Session_component, Single_client> Root_component;

	/**
	 * Root component, handling new session requests
	 */
	class Root : public Packet_root<Root_component, Session_component, CACHED>
	{
		public:

			Root(Server::Entrypoint &ep, Allocator *md_alloc,
			     Device *device)
			: Packet_root(ep, md_alloc, device) { }
	};
}


#endif /* _NIC__COMPONENT_H_ */
