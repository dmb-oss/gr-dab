/* -*- c++ -*- */
/*
 * Copyright belongs to Andreas Mueller
 * Modified 2017 by Moritz Luca Schmid, Communications Engineering Lab (CEL) / Karlsruhe Institute of Technology (KIT).
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>

#include <gnuradio/io_signature.h>
#include "fib_sink_vb_impl.h"
#include <stdexcept>
#include <stdio.h>
#include <sstream>
#include <string>
#include <boost/format.hpp>
#include "crc16.h"
#include "FIC.h"


using namespace boost;

namespace gr {
  namespace dab {

    fib_sink_vb::sptr
    fib_sink_vb::make()
    {
      return gnuradio::get_initial_sptr
              (new fib_sink_vb_impl());
    }


    fib_sink_vb_impl::fib_sink_vb_impl()
            : gr::sync_block("fib_sink_vb",
                             gr::io_signature::make(1, 1, sizeof(uint8_t) * FIB_LENGTH),
                             gr::io_signature::make(0, 0, 0))
    {
      d_service_info_written_trigger = -1;
      d_service_labels_written_trigger = -1;
      d_subch_info_written_trigger = -1;
      d_programme_type_written_trigger = -1;
      d_crc_passed = false;
      d_print_channel_info = false;
    }

    void
    fib_sink_vb_impl::set_print_channel_info(bool val)
    {
      d_print_channel_info = val;
    }

    int
    fib_sink_vb_impl::process_fib(const uint8_t *fib)
    {
      uint8_t type, length, pos;
      if (crc16((const char *)fib, FIB_LENGTH, FIB_CRC_POLY, FIB_CRC_INITSTATE) != 0) {
        GR_LOG_DEBUG(d_logger, "FIB CRC error");
        d_crc_passed = false;
        return 1;
      }
      GR_LOG_DEBUG(d_logger, "FIB correct");
      d_crc_passed = true;
      pos = 0;
      while (pos < FIB_DATA_FIELD_LENGTH && fib[pos] != FIB_ENDMARKER) {
        type = (uint8_t)(fib[pos] & 0xe0) >> 5;
        length = (uint8_t)(fib[pos] & 0x1f);
        if (length == 0 || length == 30 || length == 31
            || pos + length >= FIB_DATA_FIELD_LENGTH) {
          GR_LOG_DEBUG(d_logger, "FIG length error");
          break;
        }
        process_fig(type, &fib[pos], length);
        pos += length + 1;
      }
      return 0;
    }

    int
    fib_sink_vb_impl::process_fig(uint8_t type, const uint8_t *data, uint8_t length)
    {
      uint8_t cn, oe, pd, extension;
      switch (type) {
        case FIB_FIG_TYPE_MCI:
          extension = (uint8_t)(data[1] & 0x1f);
          GR_LOG_DEBUG(d_logger, (format("FIG %d/%d") % (int)type % (int)extension).str());
          cn = (uint8_t)(data[1] & 0x80) >> 7;
          if (cn == 1) GR_LOG_DEBUG(d_logger, "[WARNING, INFO FOR FUTURE CONFIGURATION]: ");
          oe = (uint8_t)(data[1] & 0x40) >> 6;
          if (cn == 1) GR_LOG_DEBUG(d_logger, "[WARNING, INFO FOR OTHER ENSEMBLE]");
          pd = (uint8_t)(data[1] & 0x20) >> 5;
          if (pd == 1) GR_LOG_DEBUG(d_logger, "[WARNING, LONG SERVICE IDENTIFIER]");

          switch (extension) {
            case FIB_MCI_EXTENSION_ENSEMBLE_INFO: {
              uint16_t eid = (uint16_t)data[2] << 8 | (uint16_t)data[3];
              uint8_t country_ID = (uint8_t)((data[2] & 0xf0) >> 4);
              uint16_t ensemble_reference = (uint16_t)(data[2] & 0x0f) << 8 | (uint8_t) data[3];
              uint8_t change_flag = (uint8_t)((data[4] & 0xc0) >> 6);
              uint8_t occurrence_change = data[6];
              if (change_flag != 0)
                GR_LOG_DEBUG(d_logger,
                             (format(", [CHANGE OF SUBCHANNEL OR SERVICE ORGA (%d)](at CIF %d) ") % (int) change_flag %
                             (int) occurrence_change).str());
              uint8_t alarm_flag = (uint8_t)((data[4] & 0x20) >> 5);
              if (alarm_flag == 1) GR_LOG_DEBUG(d_logger, ", [ALARM MESSAGE ACCESSIBLE] ");
              uint16_t CIF_counter = (uint16_t)((data[4] & 0x1f) * 250 + (data[5]));
              GR_LOG_DEBUG(d_logger,
                           (format("ensemble info: EId 0x%04x, CIF counter %d") % eid % CIF_counter).str());
              break;
            }
            case FIB_MCI_EXTENSION_SUBCHANNEL_ORGA: {
              uint8_t subch_counter = 0;
              GR_LOG_DEBUG(d_logger, "subchannel orga: ");
              do {
                uint8_t subchID = (uint8_t)((data[2 + subch_counter] & 0xfc) >> 2);
                uint16_t start_address = (uint16_t)((data[2 + subch_counter] & 0x03) << 8) |
                                         (uint8_t)(data[3 + subch_counter]);
                uint8_t sl_form = (uint8_t)(data[4 + subch_counter] & 0x80) >> 7;
                if (sl_form == 0) {
                  uint8_t table_switch = (uint8_t)(data[4 + subch_counter] & 0x40) >> 6;
                  if (table_switch != 0) GR_LOG_DEBUG(d_logger, " [WARNING: OTHER TABLE USED] ");
                  uint8_t table_index = (uint8_t)(data[4 + subch_counter] & 0x3f);
                  GR_LOG_DEBUG(d_logger, (format("subchID = %d , start address = %d, index %d") % (int) subchID %
                                         (int) start_address % (int) table_index).str());
                  subch_counter += 3;
                } else {
                  uint8_t option = (uint8_t)(data[4 + subch_counter] & 0x70) >> 4;
                  uint8_t protect_level = (uint8_t)((data[4 + subch_counter] & 0x0c) >> 2);
                  uint16_t subch_size = (uint16_t)((data[4 + subch_counter] & 0x03) << 8) |
                                        (uint8_t)(data[5 + subch_counter]);
                  GR_LOG_DEBUG(d_logger,
                               (format("subchID = %d , start address = %d, option %d, protect level %d, subch size %d") %
                               (int) subchID % (int) start_address % (int) option % (int) protect_level %
                               (int) subch_size).str());
                  subch_counter += 4;

                  // write sub-channel info to json
                  if (d_subch_info_written_trigger < 0) {
                    d_subch_info_written_trigger = (int) subchID;
                  } else {
                    std::stringstream ss;
                    ss << d_subch_info_current << ",{" << "\"ID\":" << (int) subchID << ",\"address\":"
                       << (int) start_address << ",\"protection\":" << (int) protect_level << ",\"size\":"
                       << (int) subch_size << "}\0";
                    d_subch_info_current = ss.str();
                    if ((int) subchID == d_subch_info_written_trigger) {
                      std::stringstream ss_json;
                      ss_json << d_subch_info_current << "]" << "\0";
                      d_subch_info_current = "\0";
                      d_json_subch_info = ss_json.str();
                      d_json_subch_info[0] = '[';
                      d_subch_info_written_trigger = -1;
                      int my_conv_table[2][4] = {{12, 8, 6, 4}, {27, 21, 18, 15}};
                      char protect_string[2][4][3] = {{"A1", "A2", "A3", "A4"}, {"B1", "B2", "B3", "B4"}};
                      if (d_print_channel_info) {
                        if (option < 2 && protect_level < 4) {
                          int bit_rate = subch_size / (my_conv_table[option][protect_level]);
                          bit_rate *= (option == 0) ? 8 : (option == 1) ? 32 : 0;
                          char *protect_level_string = protect_string[option][protect_level];
                          printf("{\"bit_rate\" : \"%d\", \"address\" : \"%d\", \"subch_size\" : \"%d\", \"protect_level\" : \"%s (%d)\"}\n", bit_rate, start_address, subch_size, protect_level_string, protect_level);
                        }
                      }
                    }
                  }
                }
              } while (1 + subch_counter < length);
              break;
            }
            case FIB_MCI_EXTENSION_SERVICE_ORGA: {
              GR_LOG_DEBUG(d_logger, "service orga: ");
              uint8_t service_counter = 1;
              do { //iterate over services
                uint32_t sid; // 16 or 32 bits
                if (pd == 1) { // 32 bits
                  sid = (uint32_t)data[service_counter + 1] << 24
                      | (uint32_t)data[service_counter + 2] << 16
                      | (uint32_t)data[service_counter + 3] << 8
                      | (uint32_t)data[service_counter + 4];
                  GR_LOG_DEBUG(d_logger, (format("SId 0x%08x ") % (int)sid).str());
                  service_counter += 4;
                } else { // 16 bits
                  sid = (uint16_t)data[service_counter + 1] << 8
                      | (uint16_t)data[service_counter + 2];
                  GR_LOG_DEBUG(d_logger, (format("SId 0x%04x ") % (int)sid).str());
                  service_counter += 2;
                }
                uint8_t local_flag = (uint8_t)((data[service_counter + 1] & 0x80) >> 7);
                if (local_flag == 1) GR_LOG_DEBUG(d_logger, "[LOCAL FLAG SET] ");
                uint8_t ca = (uint8_t)((data[service_counter + 1] & 0x70) >> 4);
                if (ca != 0) GR_LOG_DEBUG(d_logger, "[CONDITIONAL ACCESS USED] ");
                uint8_t num_service_comps = (uint8_t)(data[service_counter + 1] & 0x0f);
                GR_LOG_DEBUG(d_logger, (format("(%d components):") % (int) num_service_comps).str());
                for (int i = 0; i < num_service_comps; i++) { //iterate over service components
                  uint8_t TMID = (uint8_t)((data[service_counter + 2 + i * 2] & 0xc0) >> 6);
                  uint8_t comp_type = (uint8_t)(data[service_counter + 2 + i * 2] & 0x3f);
                  uint8_t subchID = (uint8_t)((data[service_counter + 3 + i * 2] & 0xfc) >> 2);
                  uint8_t ps = (uint8_t)((data[service_counter + 3 + i * 2] & 0x02) >> 1);
                  if (TMID == 0) {
                    GR_LOG_DEBUG(d_logger,
                                 (format("(audio stream, type %d, subchID %d, primary %d)") % (int) comp_type %
                                 (int) subchID % (int) ps).str());
                    // write service info from specififc subchannel to json
                    if (d_service_info_written_trigger < 0) {
                      d_service_info_written_trigger = (int) subchID;
                    } else {
                      std::stringstream ss;
                      ss << d_service_info_current << ",{" << "\"SId\":" << (int) sid << ",\"SubChId\":"
                         << (int) subchID << ",\"primary\":" << ((ps == 1) ? "true" : "false") << ",\"DAB+\":"
                         << (((int) comp_type == 63) ? "true" : "false") << "}\0";
                      d_service_info_current = ss.str();
                      if ((int) subchID == d_service_info_written_trigger) {
                        std::stringstream ss_json;
                        ss_json << d_service_info_current << "]" << "\0";
                        d_service_info_current = "\0";
                        d_json_service_info = ss_json.str();
                        d_json_service_info[0] = '[';
                        d_service_info_written_trigger = -1;
                      }
                    }
                  } else if (TMID == 1) {
                    GR_LOG_DEBUG(d_logger,
                                 (format("(data stream, type %d, subchID %d, primary %d)") % (int) comp_type %
                                 (int) subchID % (int) ps).str());
                  } else if (TMID == 2) {
                    GR_LOG_DEBUG(d_logger,
                                 (format("(FIDC, type %d, subchID %d, primary %d)") % (int) comp_type % (int) subchID %
                                 (int) ps).str());
                  } else {
                    GR_LOG_DEBUG(d_logger,
                                 (format("(packed data, SCId %d, primary %d)") % (int) (comp_type << 6 | subchID) % (int) ps).str());
                  }
                }
                service_counter += 1 + 2 * num_service_comps;
              } while (service_counter < length);
              break;
            }
            case FIB_MCI_EXTENSION_SERVICE_ORGA_PACKET_MODE: {
              GR_LOG_DEBUG(d_logger, "service orga packet mode: ");
              int cnt = 1;
              do { // iterate over service components
                uint16_t scid = (uint16_t)data[cnt + 1] << 4 | (uint16_t)(data[cnt + 2] & 0xf0) >> 4;
                uint8_t caorg_flag = (uint8_t)(data[cnt + 2] & 0x01);
                uint8_t dg_flag = (uint8_t)(data[cnt + 3] & 0x80) >> 7;
                uint8_t dscty = (uint8_t)(data[cnt + 3] & 0x3f);
                uint8_t subchid = (uint8_t)(data[cnt + 4] & 0xfc) >> 2;
                uint16_t packet_address = (uint16_t)(data[cnt + 4] & 0x3) << 8 | (uint16_t)data[cnt + 5];
                cnt += 5;
                uint16_t caorg;
                if (caorg_flag == 1) {
                  caorg = (uint16_t)data[cnt + 1] << 8 | (uint16_t)data[cnt + 2];
                  cnt += 2;
                }
                GR_LOG_DEBUG(d_logger, (format("SCId %d, DG flag %d, DSCTy %d, SubChId %d, Packet address %d")
                             % scid % (int)dg_flag % (int)dscty % (int)subchid % packet_address).str());
              } while (cnt < length);
              break;
            }
            case FIB_MCI_EXTENSION_SERVICE_ORGA_CA:
              GR_LOG_DEBUG(d_logger, "service orga conditional access");
              break;
            case FIB_SI_EXTENSION_SERVICE_COMP_LANGUAGE:
              GR_LOG_DEBUG(d_logger, "service comp language");
              break;
            case FIB_MCI_EXTENSION_SERVICE_COMP_GLOBAL_DEFINITION: {
              GR_LOG_DEBUG(d_logger, "service comp global definition: ");
              int cnt = 1;
              do { // iterate over service components
                uint32_t sid; // 16 or 32 bits
                if (pd == 1) { // 32 bits
                  sid = (uint32_t)data[cnt + 1] << 24
                      | (uint32_t)data[cnt + 2] << 16
                      | (uint32_t)data[cnt + 3] << 8
                      | (uint32_t)data[cnt + 4];
                  GR_LOG_DEBUG(d_logger, (format("SId 0x%08x ") % (int)sid).str());
                  cnt += 4;
                } else { // 16 bits
                  sid = (uint16_t)data[cnt + 1] << 8
                      | (uint16_t)data[cnt + 2];
                  GR_LOG_DEBUG(d_logger, (format("SId 0x%04x ") % (int)sid).str());
                  cnt += 2;
                }
                uint8_t extension_flag = (uint8_t)(data[cnt + 1] & 0x80) >> 7;
                uint8_t scids = (uint8_t)(data[cnt + 1] & 0x0f);
                uint8_t ls_flag = (uint8_t)(data[cnt + 2] & 0x80) >> 7;
                if (ls_flag == 0) {
                  uint8_t subchid = (uint8_t)(data[cnt + 2] & 0x3f);
                  GR_LOG_DEBUG(d_logger, (format("SCIdS %d, SubChId %d") % (int)scids % (int)subchid).str());
                  cnt += 2;
                } else {
                  uint16_t scid = (uint16_t)(data[cnt + 2] & 0x0f) << 8 | (uint16_t)data[cnt + 3];
                  GR_LOG_DEBUG(d_logger, (format("SCIdS %d, SCId %d") % (int)scids % (int)scid).str());
                  cnt += 3;
                }
                if (extension_flag == 1) {
                  cnt += 1;
                }
              } while (cnt < length);
              break;
            }
            case FIB_SI_EXTENSION_COUNTRY_LTO:
              GR_LOG_DEBUG(d_logger, "country LTO");
              break;
            case FIB_SI_EXTENSION_TIME_AND_COUNTRYID:
              GR_LOG_DEBUG(d_logger, "date time");
              break;
            case FIB_SI_EXTENSION_USER_APPLICATION_INFO: {
              GR_LOG_DEBUG(d_logger, "user application info: ");
              int cnt = 1;
              do { // iterate over service components
                uint32_t sid; // 16 or 32 bits
                if (pd == 1) { // 32 bits
                  sid = (uint32_t)data[cnt + 1] << 24
                      | (uint32_t)data[cnt + 2] << 16
                      | (uint32_t)data[cnt + 3] << 8
                      | (uint32_t)data[cnt + 4];
                  GR_LOG_DEBUG(d_logger, (format("SId 0x%08x ") % (int)sid).str());
                  cnt += 4;
                } else { // 16 bits
                  sid = (uint16_t)data[cnt + 1] << 8
                      | (uint16_t)data[cnt + 2];
                  GR_LOG_DEBUG(d_logger, (format("SId 0x%04x ") % (int)sid).str());
                  cnt += 2;
                }
                uint8_t scids = (uint8_t)(data[cnt + 1] & 0xf0) >> 4;
                uint8_t num_user_apps = (uint8_t)(data[cnt + 1] & 0x0f);
                GR_LOG_DEBUG(d_logger, (format("SCIdS %d ") % (int)scids).str());
                cnt += 1;
                for (int i = 0; i < num_user_apps; i++) { // iterate over user apps
                  uint16_t user_app_type = (uint16_t)data[cnt + 1] << 3 | (uint16_t)(data[cnt + 2] & 0xe0) >> 5;
                  uint8_t user_app_data_len = (uint8_t)(data[cnt + 2] & 0x1f);
                  GR_LOG_DEBUG(d_logger, (format("User App Type 0x%03x, data: ") % user_app_type).str());
                  for (int j = 0; j < user_app_data_len; j++) {
                    GR_LOG_DEBUG(d_logger, (format("%02x ") % (int)data[cnt + 3 + j]).str());
                  }
                  cnt += 2 + user_app_data_len;
                }
              } while (cnt < length);
              break;
            }
            case FIB_MCI_EXTENSION_SUBCHANNEL_PACKET_MODE_FEC:
              GR_LOG_DEBUG(d_logger, "subchannel orga packet mode fec");
              break;
            case FIB_SI_EXTENSION_PROGRAMME_NUMBER:
              GR_LOG_DEBUG(d_logger, "programme number");
              break;
            case FIB_SI_EXTENSION_PROGRAMME_TYPE: {
              GR_LOG_DEBUG(d_logger, (format("programme type, %d components") %((length-1)/4)).str());
              for(int i = 0; i < (length-1)/4; i++) {
                uint8_t programme_type = (uint8_t)(data[2 + i*4 + 3] & 0x1f);
                uint16_t service_reference = (uint16_t)(data[2 + i*4] & 0x0f) << 8 |
                                             (uint8_t) data[2 + i*4 + 1];
                GR_LOG_DEBUG(d_logger, (format("reference %d, type: %d") %service_reference %(int)programme_type).str());

                // write programme type to json
                if (d_programme_type_written_trigger < 0) {
                  d_programme_type_written_trigger = (int) service_reference;
                } else {
                  std::stringstream ss;
                  ss << d_programme_type_current << ",{" << "\"reference\":" << (int) service_reference << ",\"programme_type\":"
                     << (int) programme_type << "}\0";
                  d_programme_type_current = ss.str();
                  if ((int) service_reference == d_programme_type_written_trigger) {
                    std::stringstream ss_json;
                    ss_json << d_programme_type_current << "]" << "\0";
                    d_programme_type_current = "\0";
                    d_json_programme_type = ss_json.str();
                    d_json_programme_type[0] = '[';
                    d_programme_type_written_trigger = -1;
                  }
                }
              }
              break;
            }
            case FIB_SI_EXTENSION_ANNOUNCEMENT_SUPPORT:
              GR_LOG_DEBUG(d_logger, "announcement support");
              break;
            case FIB_SI_EXTENSION_ANNOUNCEMENT_SWITCHING:
              GR_LOG_DEBUG(d_logger, "announcement switching");
              break;
            default:
              GR_LOG_DEBUG(d_logger, (format("unsupported extension (%d)") % (int) extension).str());
              break;
          }
          break;
        case FIB_FIG_TYPE_LABEL1: {
          extension = (uint8_t)(data[1] & 0x07);
          GR_LOG_DEBUG(d_logger, (format("FIG %d/%d") % (int)type % (int)extension).str());
          char label[17];
          label[16] = '\0';
          // TODO: FIG length validation, charset, abbreviated label
          switch (extension) {
            case FIB_SI_EXTENSION_ENSEMBLE_LABEL: {
              uint16_t eid = (uint16_t)data[2] << 8 | (uint16_t)data[3];
              // uint8_t country_id = (uint16_t)(eid & 0xf000) >> 12;
              // uint16_t ensemble_reference = (uint16_t)(eid & 0x0fff);
              memcpy(label, &data[4], 16);
              GR_LOG_DEBUG(d_logger, (format("[ensemble label] (EId 0x%04x): %s") % (int)eid % label).str());
              // write json for ensemble label and ID
              std::stringstream ss;
              ss << "{" << "\"" << label << "\":{" << "\"EId\":" << (int)eid << "}}";
              d_json_ensemble_info = ss.str();
              break;
            }
            case FIB_SI_EXTENSION_PROGRAMME_SERVICE_LABEL: {
              uint16_t sid = (uint16_t)data[2] << 8 | (uint16_t)data[3];
              // uint16_t service_reference = (uint16_t)(sid & 0x0fff);
              memcpy(label, &data[4], 16);
              for (int i=0;i<16;i++) {
                if (label[i] >= 0x20 && label[i] <= 0x7e) { } // ASCII printable characters
                else label[i] = '`'; // Temporarily replacing invalid characters with ` to avoid encoding problems in Python FIXME
              }
              GR_LOG_DEBUG(d_logger, (format("[programme service label] (SId 0x%04x): %s") % (int)sid % label).str());
              // write service labels from services to json
              if (d_service_labels_written_trigger < 0) {
                d_service_labels_written_trigger = (int)sid;
              } else {
                std::stringstream ss;
                ss << d_service_labels_current << ",{" << "\"label\":\"" << label << "\",\"SId\":"
                   << (int)sid << "}\0";
                d_service_labels_current = ss.str();
                if ((int)sid == d_service_labels_written_trigger) {
                  std::stringstream ss_json;
                  ss_json << d_service_labels_current << "]" << "\0";
                  d_service_labels_current = "\0";
                  d_json_service_labels = ss_json.str();
                  d_json_service_labels[0] = '[';
                  d_service_labels_written_trigger = -1;
                }
              }
              break;
            }
            case FIB_SI_EXTENSION_SERVICE_COMP_LABEL: {
              pd = (uint8_t)(data[2] & 0x80) >> 7;
              uint8_t scids = (uint8_t)(data[2] & 0x0f);
              uint32_t sid; // 16 or 32 bits
              if (pd == 1) { // 32 bits
                sid = (uint32_t)data[3] << 24 | (uint32_t)data[4] << 16 | (uint32_t)data[5] << 8 | (uint32_t)data[6];
                memcpy(label, &data[7], 16);
                GR_LOG_DEBUG(d_logger, (format("[service component label] (SId 0x%08x, SCIdS %d): %s")
                             % (int)sid % (int)scids % label).str());
              } else { // 16 bits
                sid = (uint16_t)data[3] << 8 | (uint16_t)data[4];
                memcpy(label, &data[5], 16);
                GR_LOG_DEBUG(d_logger, (format("[service component label] (SId 0x%04x, SCIdS %d): %s")
                             % (int)sid % (int)scids % label).str());
              }
              break;
            }
            case FIB_SI_EXTENSION_DATA_SERVICE_LABEL: {
              uint32_t sid = (uint32_t)data[2] << 24 | (uint32_t)data[3] << 16 | (uint32_t)data[4] << 8 | (uint32_t)data[5];
              memcpy(label, &data[6], 16);
              GR_LOG_DEBUG(d_logger, (format("[data service label] (SId 0x%08x): %s") % (int)sid % label).str());
              break;
            }
            default:
              GR_LOG_DEBUG(d_logger, (format("[unknown extension (%d)") % (int) extension).str());
              break;
          }
          break;
        }
        case FIB_FIG_TYPE_LABEL2:
          extension = (uint8_t)(data[1] & 0x07);
          GR_LOG_DEBUG(d_logger, (format("FIG %d/%d: not supported yet") % (int)type % (int)extension).str());
          break;
        case FIB_FIG_TYPE_FIDC:
          extension = (uint8_t)(data[1] & 0x07);
          GR_LOG_DEBUG(d_logger, (format("FIG %d/%d") % (int)type % (int)extension).str());
          switch (extension) {
            case FIB_FIDC_EXTENSION_PAGING:
              GR_LOG_DEBUG(d_logger, "paging - not supported yet");
              break;
            case FIB_FIDC_EXTENSION_TMC:
              GR_LOG_DEBUG(d_logger, "TMC (traffic message channel) - not supported yet");
              break;
            case FIB_FIDC_EXTENSION_EWS:
              GR_LOG_DEBUG(d_logger, "EWS (emergency warning service) - not supported yet");
              break;
            default:
              GR_LOG_DEBUG(d_logger, (format("unsupported extension (%d)") % (int) extension).str());
          }
          break;
        case FIB_FIG_TYPE_CA:
          GR_LOG_DEBUG(d_logger, "FIG type 6 CA (conditional access): not supported yet");
          break;
        default:
          GR_LOG_DEBUG(d_logger, (format("FIG type %d: unsupported") % (int)type).str());
          break;
      }
      return 0;
    }

    int
    fib_sink_vb_impl::work(int noutput_items,
                           gr_vector_const_void_star &input_items,
                           gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t *) input_items[0];

      for (int i = 0; i < noutput_items; i++) {
        process_fib(in);
        in += FIB_LENGTH;
      }


      return noutput_items;
    }
  }
}
