// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *        http://www.samsung.com/
 */

#include <linux/module.h>
#include <soc/samsung/exynos_pm_qos.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/string.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/plist.h>

#include <linux/uaccess.h>
#include <linux/export.h>

static void exynos_plist_del(struct plist_node *node, struct plist_head *head);
static void exynos_plist_add(struct plist_node *node, struct plist_head *head);
/*
 * locking rule: all changes to constraints or notifiers lists
 * or pm_qos_object list and pm_qos_objects need to happen with pm_qos_lock
 * held, taken with _irqsave.  One lock to rule them all
 */
struct exynos_pm_qos_object {
	struct exynos_pm_qos_constraints *constraints;
	struct miscdevice exynos_pm_qos_power_miscdev;
	char *name;
};

static DEFINE_RWLOCK(exynos_pm_qos_lock);

static struct exynos_pm_qos_object null_exynos_pm_qos;
static BLOCKING_NOTIFIER_HEAD(network_lat_notifier);
static struct exynos_pm_qos_constraints network_lat_constraints = {
	.list = PLIST_HEAD_INIT(network_lat_constraints.list),
	.target_value = PM_QOS_NETWORK_LAT_DEFAULT_VALUE,
	.default_value = PM_QOS_NETWORK_LAT_DEFAULT_VALUE,
	.no_constraint_value = PM_QOS_NETWORK_LAT_DEFAULT_VALUE,
	.type = EXYNOS_PM_QOS_MIN,
	.notifiers = &network_lat_notifier,
	.lock = __SPIN_LOCK_UNLOCKED(network_lat),
};
static struct exynos_pm_qos_object network_lat_pm_qos = {
	.constraints = &network_lat_constraints,
	.name = "network_latency",
};

static BLOCKING_NOTIFIER_HEAD(device_throughput_notifier);
static struct exynos_pm_qos_constraints device_tput_constraints = {
	.list = PLIST_HEAD_INIT(device_tput_constraints.list),
	.target_value = PM_QOS_DEVICE_THROUGHPUT_DEFAULT_VALUE,
	.default_value = PM_QOS_DEVICE_THROUGHPUT_DEFAULT_VALUE,
	.type = EXYNOS_PM_QOS_MAX,
	.notifiers = &device_throughput_notifier,
	.lock = __SPIN_LOCK_UNLOCKED(device_throughput),
};
static struct exynos_pm_qos_object device_throughput_pm_qos = {
	.constraints = &device_tput_constraints,
	.name = "device_throughput",
};

static BLOCKING_NOTIFIER_HEAD(device_throughput_max_notifier);
static struct exynos_pm_qos_constraints device_tput_max_constraints = {
	.list = PLIST_HEAD_INIT(device_tput_max_constraints.list),
	.target_value = PM_QOS_DEVICE_THROUGHPUT_MAX_DEFAULT_VALUE,
	.default_value = PM_QOS_DEVICE_THROUGHPUT_MAX_DEFAULT_VALUE,
	.type = EXYNOS_PM_QOS_MIN,
	.notifiers = &device_throughput_max_notifier,
	.lock = __SPIN_LOCK_UNLOCKED(device_throughput_max),
};
static struct exynos_pm_qos_object device_throughput_max_pm_qos = {
	.constraints = &device_tput_max_constraints,
	.name = "device_throughput_max",
};

static BLOCKING_NOTIFIER_HEAD(bus_throughput_notifier);
static struct exynos_pm_qos_constraints bus_tput_constraints = {
	.list = PLIST_HEAD_INIT(bus_tput_constraints.list),
	.target_value = PM_QOS_BUS_THROUGHPUT_DEFAULT_VALUE,
	.default_value = PM_QOS_BUS_THROUGHPUT_DEFAULT_VALUE,
	.type = EXYNOS_PM_QOS_MAX,
	.notifiers = &bus_throughput_notifier,
	.lock = __SPIN_LOCK_UNLOCKED(bus_throughput),
};
static struct exynos_pm_qos_object bus_throughput_pm_qos = {
	.constraints = &bus_tput_constraints,
	.name = "bus_throughput",
};

static BLOCKING_NOTIFIER_HEAD(bus_throughput_max_notifier);
static struct exynos_pm_qos_constraints bus_tput_max_constraints = {
	.list = PLIST_HEAD_INIT(bus_tput_max_constraints.list),
	.target_value = PM_QOS_BUS_THROUGHPUT_MAX_DEFAULT_VALUE,
	.default_value = PM_QOS_BUS_THROUGHPUT_MAX_DEFAULT_VALUE,
	.type = EXYNOS_PM_QOS_MIN,
	.notifiers = &bus_throughput_max_notifier,
	.lock = __SPIN_LOCK_UNLOCKED(bus_throughput_max),
};
static struct exynos_pm_qos_object bus_throughput_max_pm_qos = {
	.constraints = &bus_tput_max_constraints,
	.name = "bus_throughput_max",
};

static struct exynos_pm_qos_object *exynos_pm_qos_array[] = {
	&null_exynos_pm_qos,
	&network_lat_pm_qos,
	&device_throughput_pm_qos,
	&device_throughput_max_pm_qos,
	&bus_throughput_pm_qos,
	&bus_throughput_max_pm_qos,
};

static ssize_t exynos_pm_qos_power_write(struct file *filp,
		const char __user *buf, size_t count, loff_t *f_pos);
static ssize_t exynos_pm_qos_power_read(struct file *filp, char __user *buf,
		size_t count, loff_t *f_pos);
static int exynos_pm_qos_power_open(struct inode *inode, struct file *filp);
static int exynos_pm_qos_power_release(struct inode *inode, struct file *filp);

static const struct file_operations exynos_pm_qos_power_fops = {
	.write = exynos_pm_qos_power_write,
	.read = exynos_pm_qos_power_read,
	.open = exynos_pm_qos_power_open,
	.release = exynos_pm_qos_power_release,
	.llseek = noop_llseek,
};

/* unlocked internal variant */
static inline int exynos_pm_qos_get_value(struct exynos_pm_qos_constraints *c)
{
	struct plist_node *node;
	int total_value = 0;

	if (plist_head_empty(&c->list))
		return c->no_constraint_value;

	switch (c->type) {
	case EXYNOS_PM_QOS_MIN:
		return plist_first(&c->list)->prio;

	case EXYNOS_PM_QOS_MAX:
	case EXYNOS_PM_QOS_FORCE_MAX:
		return plist_last(&c->list)->prio;

	case EXYNOS_PM_QOS_SUM:
		plist_for_each(node, &c->list)
			total_value += node->prio;

		return total_value;

	default:
		/* runtime check for not using enum */
		return EXYNOS_PM_QOS_DEFAULT_VALUE;
	}
}

static s32 exynos_pm_qos_read_value(struct exynos_pm_qos_constraints *c)
{
	return c->target_value;
}
/**
 * pm_qos_read_req_value - returns requested qos value
 * @pm_qos_class: identification of which qos value is requested
 * @req: request wanted to find set value
 *
 * This function returns the requested qos value by sysfs node.
 */
int exynos_pm_qos_read_req_value(int pm_qos_class, struct exynos_pm_qos_request *req)
{
	struct plist_node *p;
	unsigned long flags;

	read_lock_irqsave(&exynos_pm_qos_lock, flags);

	if (pm_qos_class < 0) {
		return -EINVAL;
	}
	plist_for_each(p, &exynos_pm_qos_array[pm_qos_class]->constraints->list) {
		if (req == container_of(p, struct exynos_pm_qos_request, node)) {
			read_unlock_irqrestore(&exynos_pm_qos_lock, flags);
			return p->prio;
		}
	}

	read_unlock_irqrestore(&exynos_pm_qos_lock, flags);

	return -ENODATA;
}
EXPORT_SYMBOL_GPL(exynos_pm_qos_read_req_value);

static inline void exynos_pm_qos_set_value(struct exynos_pm_qos_constraints *c, s32 value)
{
	c->target_value = value;
}

static int pm_qos_dbg_show_requests(struct seq_file *s, void *unused)
{
	struct exynos_pm_qos_object *qos = (struct exynos_pm_qos_object *)s->private;
	struct exynos_pm_qos_constraints *c;
	struct exynos_pm_qos_request *req;
	char *type;
	unsigned long flags;
	int tot_reqs = 0;
	int active_reqs = 0;

	if (IS_ERR_OR_NULL(qos)) {
		dev_err(qos->exynos_pm_qos_power_miscdev.this_device,
				"%s: bad qos param!\n", __func__);
		return -EINVAL;
	}
	c = qos->constraints;
	if (IS_ERR_OR_NULL(c)) {
		dev_err(qos->exynos_pm_qos_power_miscdev.this_device,
				"%s: Bad constraints on qos?\n", __func__);
		return -EINVAL;
	}

	/* Lock to ensure we have a snapshot */
	write_lock_irqsave(&exynos_pm_qos_lock, flags);
	if (plist_head_empty(&c->list)) {
		seq_puts(s, "Empty!\n");
		goto out;
	}

	switch (c->type) {
	case EXYNOS_PM_QOS_MIN:
		type = "Minimum";
		break;
	case EXYNOS_PM_QOS_MAX:
		type = "Maximum";
		break;
	case EXYNOS_PM_QOS_SUM:
		type = "Sum";
		break;
	default:
		type = "Unknown";
	}

	plist_for_each_entry(req, &c->list, node) {
		char *state = "Default";

		if ((req->node).prio != c->default_value) {
			active_reqs++;
			state = "Active";
		}
		tot_reqs++;
		seq_printf(s, "%d: %d: %s(%s:%d)\n", tot_reqs,
			   (req->node).prio, state,
			   req->func,
			   req->line);
	}

	seq_printf(s, "Type=%s, Value=%d, Requests: active=%d / total=%d\n",
		   type, exynos_pm_qos_get_value(c), active_reqs, tot_reqs);

out:
	write_unlock_irqrestore(&exynos_pm_qos_lock, flags);
	return 0;
}

static int exynos_pm_qos_dbg_open(struct inode *inode, struct file *file)
{
	return single_open(file, pm_qos_dbg_show_requests, inode->i_private);
}

static const struct file_operations exynos_pm_qos_debug_fops = {
	.open           = exynos_pm_qos_dbg_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	.release        = single_release,
};

/**
 * pm_qos_update_target - manages the constraints list and calls the notifiers
 *  if needed
 * @c: constraints data struct
 * @node: request to add to the list, to update or to remove
 * @action: action to take on the constraints list
 * @value: value of the request to add or update
 *
 * This function returns 1 if the aggregated constraint value has changed, 0
 *  otherwise.
 */
int exynos_pm_qos_update_target(struct exynos_pm_qos_constraints *c,
				struct plist_node *node,
				enum exynos_pm_qos_req_action action, int value)
{
	unsigned long flags;
	int prev_value, curr_value, new_value;
	int ret;

	read_lock_irqsave(&exynos_pm_qos_lock, flags);
	spin_lock(&c->lock);
	prev_value = exynos_pm_qos_get_value(c);
	if (value == EXYNOS_PM_QOS_DEFAULT_VALUE)
		new_value = c->default_value;
	else
		new_value = value;

	switch (action) {
	case EXYNOS_PM_QOS_REMOVE_REQ:
		exynos_plist_del(node, &c->list);
		break;
	case EXYNOS_PM_QOS_UPDATE_REQ:
		/*
		 * to change the list, we atomically remove, reinit
		 * with new value and add, then see if the extremal
		 * changed
		 */
		exynos_plist_del(node, &c->list);
		fallthrough;
	case EXYNOS_PM_QOS_ADD_REQ:
		plist_node_init(node, new_value);
		exynos_plist_add(node, &c->list);
		break;
	default:
		/* no action */
		break;
	}

	curr_value = exynos_pm_qos_get_value(c);
	exynos_pm_qos_set_value(c, curr_value);

	spin_unlock(&c->lock);
	read_unlock_irqrestore(&exynos_pm_qos_lock, flags);

	if (c->type == EXYNOS_PM_QOS_FORCE_MAX) {
		blocking_notifier_call_chain(c->notifiers,
					     (unsigned long)curr_value,
					     NULL);
		return 1;
	}

	if (prev_value != curr_value) {
		struct exynos_pm_qos_request *req = container_of(node, struct exynos_pm_qos_request, node);

		ret = 1;
		if (c->notifiers)
			blocking_notifier_call_chain(c->notifiers,
						     (unsigned long)curr_value,
						     (void *)&req->exynos_pm_qos_class);
	} else {
		ret = 0;
	}
	return ret;
}

/**
 * pm_qos_flags_remove_req - Remove device PM QoS flags request.
 * @pqf: Device PM QoS flags set to remove the request from.
 * @req: Request to remove from the set.
 */
static void exynos_pm_qos_flags_remove_req(struct exynos_pm_qos_flags *pqf,
				    struct exynos_pm_qos_flags_request *req)
{
	s32 val = 0;

	list_del(&req->node);
	list_for_each_entry(req, &pqf->list, node)
		val |= req->flags;

	pqf->effective_flags = val;
}

/**
 * pm_qos_update_flags - Update a set of PM QoS flags.
 * @pqf: Set of flags to update.
 * @req: Request to add to the set, to modify, or to remove from the set.
 * @action: Action to take on the set.
 * @val: Value of the request to add or modify.
 *
 * Update the given set of PM QoS flags and call notifiers if the aggregate
 * value has changed.  Returns 1 if the aggregate constraint value has changed,
 * 0 otherwise.
 */
bool exynos_pm_qos_update_flags(struct exynos_pm_qos_flags *pqf,
			 struct exynos_pm_qos_flags_request *req,
			 enum exynos_pm_qos_req_action action, s32 val)
{
	unsigned long irqflags;
	s32 prev_value, curr_value;

	read_lock_irqsave(&exynos_pm_qos_lock, irqflags);

	prev_value = list_empty(&pqf->list) ? 0 : pqf->effective_flags;

	switch (action) {
	case EXYNOS_PM_QOS_REMOVE_REQ:
		exynos_pm_qos_flags_remove_req(pqf, req);
		break;
	case EXYNOS_PM_QOS_UPDATE_REQ:
		exynos_pm_qos_flags_remove_req(pqf, req);
		fallthrough;
	case EXYNOS_PM_QOS_ADD_REQ:
		req->flags = val;
		INIT_LIST_HEAD(&req->node);
		list_add_tail(&req->node, &pqf->list);
		pqf->effective_flags |= val;
		break;
	default:
		/* no action */
		break;
	}

	curr_value = list_empty(&pqf->list) ? 0 : pqf->effective_flags;

	read_unlock_irqrestore(&exynos_pm_qos_lock, irqflags);

	return prev_value != curr_value;
}

/**
 * pm_qos_request - returns current system wide qos expectation
 * @pm_qos_class: identification of which qos value is requested
 *
 * This function returns the current target value.
 */
int exynos_pm_qos_request(int exynos_pm_qos_class)
{
	if (exynos_pm_qos_class < 0)
		return -EINVAL;
	return exynos_pm_qos_read_value(exynos_pm_qos_array[exynos_pm_qos_class]->constraints);
}
EXPORT_SYMBOL_GPL(exynos_pm_qos_request);

int exynos_pm_qos_request_active(struct exynos_pm_qos_request *req)
{
	return req->exynos_pm_qos_class != 0;
}
EXPORT_SYMBOL_GPL(exynos_pm_qos_request_active);

static void __exynos_pm_qos_update_request(struct exynos_pm_qos_request *req,
			   s32 new_value)
{
	if (new_value != req->node.prio) {
		if (req->exynos_pm_qos_class < 0)
			return;
		exynos_pm_qos_update_target(
			exynos_pm_qos_array[req->exynos_pm_qos_class]->constraints,
			&req->node, EXYNOS_PM_QOS_UPDATE_REQ, new_value);
	}
}

/**
 * pm_qos_work_fn - the timeout handler of pm_qos_update_request_timeout
 * @work: work struct for the delayed work (timeout)
 *
 * This cancels the timeout request by falling back to the default at timeout.
 */
static void exynos_pm_qos_work_fn(struct work_struct *work)
{
	struct exynos_pm_qos_request *req = container_of(to_delayed_work(work),
						  struct exynos_pm_qos_request,
						  work);

	__exynos_pm_qos_update_request(req, EXYNOS_PM_QOS_DEFAULT_VALUE);
}

/**
 * pm_qos_add_request - inserts new qos request into the list
 * @req: pointer to a preallocated handle
 * @pm_qos_class: identifies which list of qos request to use
 * @value: defines the qos request
 *
 * This function inserts a new entry in the pm_qos_class list of requested qos
 * performance characteristics.  It recomputes the aggregate QoS expectations
 * for the pm_qos_class of parameters and initializes the pm_qos_request
 * handle.  Caller needs to save this handle for later use in updates and
 * removal.
 */

void exynos_pm_qos_add_request_trace(char *func, unsigned int line,
					struct exynos_pm_qos_request *req,
					int exynos_pm_qos_class, s32 value)
{
	if (!req) /*guard against callers passing in null */
		return;

	if (exynos_pm_qos_request_active(req)) {
		WARN(1, KERN_ERR "%s() called for already added request\n", __func__);
		return;
	}
	req->exynos_pm_qos_class = exynos_pm_qos_class;
	req->func = func;
	req->line = line;
	INIT_DELAYED_WORK(&req->work, exynos_pm_qos_work_fn);
	if (exynos_pm_qos_class < 0)
		return;
	exynos_pm_qos_update_target(exynos_pm_qos_array[exynos_pm_qos_class]->constraints,
			     &req->node, EXYNOS_PM_QOS_ADD_REQ, value);
}
EXPORT_SYMBOL_GPL(exynos_pm_qos_add_request_trace);

/**
 * pm_qos_update_request - modifies an existing qos request
 * @req : handle to list element holding a pm_qos request to use
 * @value: defines the qos request
 *
 * Updates an existing qos request for the pm_qos_class of parameters along
 * with updating the target pm_qos_class value.
 *
 * Attempts are made to make this code callable on hot code paths.
 */
void exynos_pm_qos_update_request(struct exynos_pm_qos_request *req,
			   s32 new_value)
{
	if (!req) /*guard against callers passing in null */
		return;

	if (!exynos_pm_qos_request_active(req)) {
		WARN(1, KERN_ERR "%s() called for unknown object\n", __func__);
		return;
	}

	if (delayed_work_pending(&req->work))
		cancel_delayed_work_sync(&req->work);

	__exynos_pm_qos_update_request(req, new_value);
}
EXPORT_SYMBOL_GPL(exynos_pm_qos_update_request);

/**
 * pm_qos_update_request_timeout - modifies an existing qos request temporarily.
 * @req : handle to list element holding a pm_qos request to use
 * @new_value: defines the temporal qos request
 * @timeout_us: the effective duration of this qos request in usecs.
 *
 * After timeout_us, this qos request is cancelled automatically.
 */
void exynos_pm_qos_update_request_timeout(struct exynos_pm_qos_request *req, s32 new_value,
				   unsigned long timeout_us)
{
	if (!req)
		return;
	if (WARN(!exynos_pm_qos_request_active(req),
		 "%s called for unknown object.", __func__))
		return;

	if (delayed_work_pending(&req->work))
		cancel_delayed_work_sync(&req->work);

	if (new_value != req->node.prio) {
		if (req->exynos_pm_qos_class < 0)
			return;
		exynos_pm_qos_update_target(
			exynos_pm_qos_array[req->exynos_pm_qos_class]->constraints,
			&req->node, EXYNOS_PM_QOS_UPDATE_REQ, new_value);
	}

	schedule_delayed_work(&req->work, usecs_to_jiffies(timeout_us));
}
EXPORT_SYMBOL_GPL(exynos_pm_qos_update_request_timeout);

/**
 * pm_qos_remove_request - modifies an existing qos request
 * @req: handle to request list element
 *
 * Will remove pm qos request from the list of constraints and
 * recompute the current target value for the pm_qos_class.  Call this
 * on slow code paths.
 */
void exynos_pm_qos_remove_request(struct exynos_pm_qos_request *req)
{
	if (!req) /*guard against callers passing in null */
		return;
		/* silent return to keep pcm code cleaner */

	if (!exynos_pm_qos_request_active(req)) {
		WARN(1, KERN_ERR "%s() called for unknown object\n", __func__);
		return;
	}

	if (delayed_work_pending(&req->work))
		cancel_delayed_work_sync(&req->work);

	if (req->exynos_pm_qos_class < 0)
		return;
	exynos_pm_qos_update_target(exynos_pm_qos_array[req->exynos_pm_qos_class]->constraints,
			     &req->node, EXYNOS_PM_QOS_REMOVE_REQ,
			     EXYNOS_PM_QOS_DEFAULT_VALUE);
	memset(req, 0, sizeof(*req));
}
EXPORT_SYMBOL_GPL(exynos_pm_qos_remove_request);

/* User space interface to PM QoS classes via misc devices */
static int register_pm_qos_misc(struct exynos_pm_qos_object *qos, struct dentry *d)
{
	qos->exynos_pm_qos_power_miscdev.minor = MISC_DYNAMIC_MINOR;
	qos->exynos_pm_qos_power_miscdev.name = qos->name;
	qos->exynos_pm_qos_power_miscdev.fops = &exynos_pm_qos_power_fops;

#ifdef CONFIG_DEBUG_FS
	if (d) {
		(void)debugfs_create_file(qos->name, 0444, d,
					  (void *)qos, &exynos_pm_qos_debug_fops);
	}
#endif

	return misc_register(&qos->exynos_pm_qos_power_miscdev);
}
/**
 * pm_qos_add_notifier - sets notification entry for changes to target value
 * @pm_qos_class: identifies which qos target changes should be notified.
 * @notifier: notifier block managed by caller.
 *
 * will register the notifier into a notification chain that gets called
 * upon changes to the pm_qos_class target value.
 */
int exynos_pm_qos_add_notifier(int exynos_pm_qos_class, struct notifier_block *notifier)
{
	int retval;

	if (exynos_pm_qos_class < 0)
		return -EINVAL;
	retval = blocking_notifier_chain_register(
			exynos_pm_qos_array[exynos_pm_qos_class]->constraints->notifiers,
			notifier);

	return retval;
}
EXPORT_SYMBOL_GPL(exynos_pm_qos_add_notifier);

/**
 * pm_qos_remove_notifier - deletes notification entry from chain.
 * @pm_qos_class: identifies which qos target changes are notified.
 * @notifier: notifier block to be removed.
 *
 * will remove the notifier from the notification chain that gets called
 * upon changes to the pm_qos_class target value.
 */
int exynos_pm_qos_remove_notifier(int exynos_pm_qos_class, struct notifier_block *notifier)
{
	int retval;

	if (exynos_pm_qos_class < 0)
		return -EINVAL;
	retval = blocking_notifier_chain_unregister(
			exynos_pm_qos_array[exynos_pm_qos_class]->constraints->notifiers,
			notifier);

	return retval;
}
EXPORT_SYMBOL_GPL(exynos_pm_qos_remove_notifier);

static int find_exynos_pm_qos_object_by_minor(int minor)
{
	int exynos_pm_qos_class;

	for (exynos_pm_qos_class = PM_QOS_NETWORK_LATENCY;
		exynos_pm_qos_class < EXYNOS_PM_QOS_NUM_CLASSES; exynos_pm_qos_class++) {
		if (minor ==
			exynos_pm_qos_array[exynos_pm_qos_class]->exynos_pm_qos_power_miscdev.minor)
			return exynos_pm_qos_class;
	}
	return -1;
}

static int exynos_pm_qos_power_open(struct inode *inode, struct file *filp)
{
	long exynos_pm_qos_class;

	exynos_pm_qos_class = find_exynos_pm_qos_object_by_minor(iminor(inode));
	if (exynos_pm_qos_class >= PM_QOS_NETWORK_LATENCY) {
		struct exynos_pm_qos_request *req = kzalloc(sizeof(*req), GFP_KERNEL);

		if (!req)
			return -ENOMEM;

		exynos_pm_qos_add_request(req, exynos_pm_qos_class, EXYNOS_PM_QOS_DEFAULT_VALUE);
		filp->private_data = req;

		return 0;
	}
	return -EPERM;
}

static int exynos_pm_qos_power_release(struct inode *inode, struct file *filp)
{
	struct exynos_pm_qos_request *req;

	req = filp->private_data;
	exynos_pm_qos_remove_request(req);
	kfree(req);

	return 0;
}


static ssize_t exynos_pm_qos_power_read(struct file *filp, char __user *buf,
		size_t count, loff_t *f_pos)
{
	s32 value;
	unsigned long flags;
	struct exynos_pm_qos_request *req = filp->private_data;

	if (!req)
		return -EINVAL;
	if (!exynos_pm_qos_request_active(req))
		return -EINVAL;

	if (req->exynos_pm_qos_class < 0)
		return -EINVAL;
	read_lock_irqsave(&exynos_pm_qos_lock, flags);
	value = exynos_pm_qos_get_value(exynos_pm_qos_array[req->exynos_pm_qos_class]->constraints);
	read_unlock_irqrestore(&exynos_pm_qos_lock, flags);

	return simple_read_from_buffer(buf, count, f_pos, &value, sizeof(s32));
}

static ssize_t exynos_pm_qos_power_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *f_pos)
{
	s32 value;
	struct exynos_pm_qos_request *req;

	if (count == sizeof(s32)) {
		if (copy_from_user(&value, buf, sizeof(s32)))
			return -EFAULT;
	} else {
		int ret;

		ret = kstrtos32_from_user(buf, count, 16, &value);
		if (ret)
			return ret;
	}

	req = filp->private_data;
	exynos_pm_qos_update_request(req, value);

	return count;
}

static int exynos_pm_qos_power_init(void)
{
	int ret = 0;
	int i;
	struct dentry *d;

	BUILD_BUG_ON(ARRAY_SIZE(exynos_pm_qos_array) != EXYNOS_PM_QOS_NUM_CLASSES);

#ifdef CONFIG_DEBUG_FS
	d = debugfs_create_dir("exynos_pm_qos", NULL);
#endif
	for (i = PM_QOS_NETWORK_LATENCY; i < EXYNOS_PM_QOS_NUM_CLASSES; i++) {
		ret = register_pm_qos_misc(exynos_pm_qos_array[i], d);
		if (ret < 0) {
			pr_err("%s: %s setup failed\n",
			       __func__, exynos_pm_qos_array[i]->name);
			return ret;
		}
	}
	pr_info("%s() Successfully Called!\n", __func__);

	return ret;
}
late_initcall(exynos_pm_qos_power_init);

# define plist_check_head(h)	do { } while (0)
/**
 * plist_add - add @node to @head
 *
 * @node:	&struct plist_node pointer
 * @head:	&struct plist_head pointer
 */
static void exynos_plist_add(struct plist_node *node, struct plist_head *head)
{
	struct plist_node *first, *iter, *prev = NULL;
	struct list_head *node_next = &head->node_list;

	plist_check_head(head);
	WARN_ON(!plist_node_empty(node));
	WARN_ON(!list_empty(&node->prio_list));

	if (plist_head_empty(head))
		goto ins_node;

	first = iter = plist_first(head);

	do {
		if (node->prio < iter->prio) {
			node_next = &iter->node_list;
			break;
		}

		prev = iter;
		iter = list_entry(iter->prio_list.next,
				struct plist_node, prio_list);
	} while (iter != first);

	if (!prev || prev->prio != node->prio)
		list_add_tail(&node->prio_list, &iter->prio_list);
ins_node:
	list_add_tail(&node->node_list, node_next);

	plist_check_head(head);
}

/**
 * plist_del - Remove a @node from plist.
 *
 * @node:	&struct plist_node pointer - entry to be removed
 * @head:	&struct plist_head pointer - list head
 */
static void exynos_plist_del(struct plist_node *node, struct plist_head *head)
{
	plist_check_head(head);

	if (!list_empty(&node->prio_list)) {
		if (node->node_list.next != &head->node_list) {
			struct plist_node *next;

			next = list_entry(node->node_list.next,
					struct plist_node, node_list);

			/* add the next plist_node into prio_list */
			if (list_empty(&next->prio_list))
				list_add(&next->prio_list, &node->prio_list);
		}
		list_del_init(&node->prio_list);
	}

	list_del_init(&node->node_list);

	plist_check_head(head);
}

MODULE_DESCRIPTION("Samsung EXYNOS Soc series pm_qos driver");
MODULE_LICENSE("GPL v2");
