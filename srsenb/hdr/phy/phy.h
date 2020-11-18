/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSENB_PHY_H
#define SRSENB_PHY_H

#include "phy_common.h"
#include "lte/sf_worker.h"
#include "srsenb/hdr/phy/enb_phy_base.h"
#include "srslte/common/log.h"
#include "srslte/common/log_filter.h"
#include "srslte/common/trace.h"
#include "srslte/interfaces/enb_interfaces.h"
#include "srslte/interfaces/enb_metrics_interface.h"
#include "srslte/interfaces/radio_interfaces.h"
#include "srslte/radio/radio.h"
#include "txrx.h"

namespace srsenb {

class phy final : public enb_phy_base, public phy_interface_stack_lte, public srslte::phy_interface_radio
{
public:
  phy(srslte::logger* logger_);
  ~phy();

  int  init(const phy_args_t&            args,
            const phy_cfg_t&             cfg,
            srslte::radio_interface_phy* radio_,
            stack_interface_phy_lte*     stack_);
  void stop() override;

  std::string get_type() override { return "lte"; };

  /* MAC->PHY interface */
  void rem_rnti(uint16_t rnti) final;
  int  pregen_sequences(uint16_t rnti) override;
  void set_mch_period_stop(uint32_t stop) final;
  void set_activation_deactivation_scell(uint16_t                                     rnti,
                                         const std::array<bool, SRSLTE_MAX_CARRIERS>& activation) override;

  /*RRC-PHY interface*/
  void configure_mbsfn(srslte::sib2_mbms_t* sib2, srslte::sib13_t* sib13, const srslte::mcch_msg_t& mcch) override;

  void start_plot() override;
  void set_config(uint16_t rnti, const phy_rrc_cfg_list_t& phy_cfg_list) override;
  void complete_config(uint16_t rnti) override;

  void get_metrics(std::vector<phy_metrics_t>& metrics) override;

  void cmd_cell_gain(uint32_t cell_id, float gain_db) override;

  void radio_overflow() override{};
  void radio_failure() override{};

  void srslte_phy_logger(phy_logger_level_t log_level, char* str);

private:
  srslte::phy_cfg_mbsfn_t mbsfn_config = {};
  uint32_t        nof_workers  = 0;

  const static int MAX_WORKERS = 4;

  const static int PRACH_WORKER_THREAD_PRIO = 5;
  const static int SF_RECV_THREAD_PRIO      = 1;
  const static int WORKERS_THREAD_PRIO      = 2;

  srslte::radio_interface_phy* radio = nullptr;

  srslte::logger*                                   logger = nullptr;
  std::unique_ptr<srslte::log_filter>               log_h         = nullptr;
  std::unique_ptr<srslte::log_filter>               log_phy_lib_h = nullptr;

  lte::worker_pool  lte_workers;
  phy_common        workers_common;
  prach_worker_pool prach;
  txrx              tx_rx;

  bool initialized = false;

  srslte_prach_cfg_t prach_cfg = {};

  void parse_common_config(const phy_cfg_t& cfg);
};

} // namespace srsenb

#endif // SRSENB_PHY_H
