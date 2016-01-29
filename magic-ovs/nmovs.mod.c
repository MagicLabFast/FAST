#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x9c3f02a1, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xa53ee99b, __VMLINUX_SYMBOL_STR(dev_get_by_name) },
	{ 0x4d405db8, __VMLINUX_SYMBOL_STR(param_ops_string) },
	{ 0xde458b66, __VMLINUX_SYMBOL_STR(__netdev_alloc_skb) },
	{ 0x50eedeb8, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xc9d25dfa, __VMLINUX_SYMBOL_STR(free_netdev) },
	{ 0x8bf2f356, __VMLINUX_SYMBOL_STR(register_netdev) },
	{ 0xb4390f9a, __VMLINUX_SYMBOL_STR(mcount) },
	{ 0x47da671c, __VMLINUX_SYMBOL_STR(netif_receive_skb) },
	{ 0x16305289, __VMLINUX_SYMBOL_STR(warn_slowpath_null) },
	{ 0xf5609e02, __VMLINUX_SYMBOL_STR(skb_push) },
	{ 0x221df4d5, __VMLINUX_SYMBOL_STR(dev_remove_pack) },
	{ 0xc6c7f902, __VMLINUX_SYMBOL_STR(skb_pull) },
	{ 0xe8efc5e1, __VMLINUX_SYMBOL_STR(init_net) },
	{ 0x6320ce4d, __VMLINUX_SYMBOL_STR(eth_type_trans) },
	{ 0x2e60bace, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0x54180338, __VMLINUX_SYMBOL_STR(unregister_netdev) },
	{ 0x7b496939, __VMLINUX_SYMBOL_STR(dev_add_pack) },
	{ 0xb054d439, __VMLINUX_SYMBOL_STR(consume_skb) },
	{ 0xb16c81f2, __VMLINUX_SYMBOL_STR(skb_put) },
	{ 0xe914e41e, __VMLINUX_SYMBOL_STR(strcpy) },
	{ 0x370a7749, __VMLINUX_SYMBOL_STR(alloc_etherdev_mqs) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "8CABC3FB148BC44357D8300");
