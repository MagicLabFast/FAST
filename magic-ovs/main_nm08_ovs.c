#ifndef _KERNEL_
#define _KERNEL_
#endif
#ifndef MODULE
#define MODULE
#endif

#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/in.h>
#include <linux/netdevice.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/socket.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/ip.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <net/dst.h>
#include <net/sock.h>
#include <net/ip.h>
#include <net/dsa.h>
#include <asm/uaccess.h>


MODULE_AUTHOR("Test Author (sxt)");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module with a receive  packet");

#define PORT_CNT 8
#define ETH_NM_OVS 0x0609
#define MTU 1500
#define NM_PKT_SIZE 1600

typedef u_int64_t u64;
typedef u_int32_t u32;
typedef u_int16_t u16;
typedef u_int8_t  u8;

struct nm_packet
{
	u16 port;
	struct  ethhdr eth;
	u8 data[MTU];
};

struct nm_adapter
{
	struct net_device *netdev;
	u16 port;
	u8 pad[6];
	struct net_device_stats stats;
};


char ctlif_name[16] = "eth0";
module_param_string(ctlif_name,ctlif_name,16,0);


struct net_device *nm_dev[PORT_CNT];
struct nm_adapter *adapter[PORT_CNT];
struct net_device *nm_ctl_netdev = NULL; 

void show_skb(struct sk_buff *skb)
{
        int i=0,len=0;

        printk("-----------------------*******-----------------------\n");
        printk("skb info:\n");
        printk("head:0x%p,end:0x%08lX,data:0x%p,tail:0x%08lX,len:%d\n",skb->head,(unsigned long)skb->end,skb->data,(unsigned long)skb->tail, skb->len);
        printk("skb data:\n");
        for(i=0;i<16;i++)
        {
                if(i % 16 == 0)
                        printk("      ");
                printk(" %X ",i);
                if(i % 16 == 15)
                        printk("\n");
        }
        len=skb->len;
        for(i=0;i<len;i++)
        {
                if(i % 16 == 0)
                        printk("%04X: ",i);
                printk("%02X ",*(skb->data+i));
                if(i % 16 == 15)
                        printk("\n");
        }
        if(len % 16 !=0)
                printk("\n");
        printk("-----------------------*******-----------------------\n\n");
}

/*报文显示函数*/


int eth0_pack_rcv(struct sk_buff *skb, struct net_device *dev,struct packet_type *pt, struct net_device *orig_dev);//报文接收函数
struct ethhdr xeth =
{
	.h_dest = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
	.h_source = {0x0},
	.h_proto = cpu_to_be16(ETH_NM_OVS)
};


static struct packet_type eth_pack =
{
	.type	= cpu_to_be16(ETH_NM_OVS),	//参数ETH_P_ALL代表接收所有类型报文
    .dev	= NULL,
    .func	= eth0_pack_rcv				//回调函数
};

int eth0_pack_rcv(struct sk_buff *skb,struct net_device *dev,struct packet_type *pt, struct net_device *orig_dev)
{
	struct nm_packet *pkt = (struct nm_packet *)skb->data;
	struct net_device *dev_nm = nm_dev[ntohs(pkt->port)];
	struct nm_adapter *adapter = netdev_priv(dev_nm);
	struct sk_buff *new_skb;
	//printk("skb:%p,len:%d,skb->protocol:%X,skb_type:%d,head-data:%d, head:0x%lx, data:0x%lx\n",
				//	skb,skb->len,skb->protocol,skb->pkt_type,skb->head-skb->data, 
				//(unsigned long)skb->head, (unsigned long)skb->data);

	skb_pull(skb,sizeof(pkt->port));

	new_skb = netdev_alloc_skb(dev_nm,NM_PKT_SIZE);
	skb_reserve(new_skb,2);
	memcpy(skb_put(new_skb,skb->len),skb->data,skb->len);
	adapter->stats.rx_packets++;
	adapter->stats.rx_bytes += skb->len;
	new_skb->protocol = eth_type_trans(new_skb, dev_nm);
	//printk("%s recv pkt ! skb protocol:0x%04X,len:%d\n",dev_nm->name,new_skb->protocol,skb->len);
	dev_kfree_skb(skb);
	netif_receive_skb(new_skb);
	return 0;
}

int nm_open(struct net_device *netdev)
{	
	printk("%s nm_dev_open\n",netdev->name);	
	netif_start_queue(netdev);
	return 0;
}

int nm_close(struct net_device *netdev)
{	
	printk("%s nm_dev_release\n",netdev->name);
	netif_stop_queue(netdev);
	return 0;
}

void skb_set_eth(struct sk_buff *skb)
{
	struct ethhdr *eth = NULL;

	skb_push(skb,sizeof(*eth));
	eth = (struct ethhdr *)skb->data;
	*eth = xeth;
}

void skb_set_nm_port(struct sk_buff *skb,u16 port)
{
	struct nm_packet *pkt = (struct nm_packet *)(skb->data - sizeof(pkt->port));
	pkt->port = htons(port);
	skb_push(skb,sizeof(pkt->port));
}

int nm_xmit_frame(struct sk_buff *skb, struct net_device *netdev)
{    
	struct nm_adapter *adapter = netdev_priv(netdev);
	struct nm_packet *pkt = (struct nm_packet *)(skb->data - sizeof(pkt->port));
	struct sk_buff *new_skb = NULL;

	adapter->stats.tx_packets++;
	adapter->stats.tx_bytes += skb->len;

	new_skb = netdev_alloc_skb(nm_ctl_netdev,NM_PKT_SIZE);
	skb_reserve(new_skb,2 + 16);
	skb_put(new_skb,skb->len);
	memcpy(new_skb->data,skb->data,skb->len);
	skb_set_nm_port(new_skb,adapter->port);
	skb_set_eth(new_skb);
	show_skb(new_skb);
	//printk("%s xmit!send skb protocol:0x%04X,len:%d\n",netdev->name,skb->protocol,skb->len);
	dev_kfree_skb(skb);

	nm_ctl_netdev->netdev_ops->ndo_start_xmit(new_skb,nm_ctl_netdev);
	return 0;
}

static struct net_device_stats *nm_get_stats(struct net_device *netdev)
{
	struct nm_adapter *adapter = netdev_priv(netdev);

	return &adapter->stats;
}


static const struct net_device_ops nm_netdev_ops = {
	.ndo_open		= nm_open,
	.ndo_stop		= nm_close,
	.ndo_start_xmit		= nm_xmit_frame,
	.ndo_get_stats		= nm_get_stats
};


int init_module(void)
{
	int ret = 0;
	int i=0;
	
	nm_ctl_netdev = dev_get_by_name(&init_net,ctlif_name);
	
	if(nm_ctl_netdev == NULL)
	{
		printk("%s not found!\n", ctlif_name);
		return -1;/*定义一个值，表示找不到接口*/
	}

	for(i=0;i<PORT_CNT;i++)
	{
		nm_dev[i]=alloc_etherdev(sizeof(struct nm_adapter));
		if (nm_dev[i] == NULL)
			goto rollback_alloc;

		adapter[i] = netdev_priv(nm_dev[i]);
		adapter[i]->netdev = nm_dev[i];
		adapter[i]->port = i;
		nm_dev[i]->netdev_ops = &nm_netdev_ops; 
		memcpy(nm_dev[i]->dev_addr,"06090",5);
		nm_dev[i]->dev_addr[0] = 0;
		nm_dev[i]->dev_addr[5] = i;
		strcpy(nm_dev[i]->name, "nm%d");		
		
		if ((ret = register_netdev(nm_dev[i])))
			goto rollback_reg;
	}	
	eth_pack.dev = nm_ctl_netdev;
	dev_add_pack(&eth_pack);
rollback_alloc:
	
rollback_reg:
	
    return 0;
}

void cleanup_module(void)
{
	int i=0;
	dev_remove_pack(&eth_pack);
	for(i=0;i<PORT_CNT;i++)
	{
		if (nm_dev[i])
		{		
			unregister_netdev(nm_dev[i]);
			free_netdev(nm_dev[i]);
		}
	}
	dev_put(nm_ctl_netdev);
}

