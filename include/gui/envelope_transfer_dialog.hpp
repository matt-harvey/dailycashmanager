/*
 * Copyright 2013 Matthew Harvey
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GUARD_envelope_transfer_dialog_hpp_5707053959233246
#define GUARD_envelope_transfer_dialog_hpp_5707053959233246

#include <wx/dialog.h>

namespace phatbooks
{
namespace gui
{

/**
 * @todo HIGH PRIORITY Finish this.
 */
class EnvelopeTransferDialog: public wxDialog
{
public:
	EnvelopeTransferDialog() = delete;
	EnvelopeTransferDialog(EnvelopeTransferDialog const& rhs) = delete;
	EnvelopeTransferDialog(EnvelopeTransferDialog&& rhs) = delete;
	EnvelopeTransferDialog& operator=(EnvelopeTransferDialog const& rhs) =
		delete;
	EnvelopeTransferDialog& operator=(EnvelopeTransferDialog&& rhs) = delete;
	virtual ~EnvelopeTransferDialog();

};  // class EnvelopeTransferDialog

}  // namespace gui
}  // namespace phatbooks

#endif  // GUARD_envelope_transfer_dialog_hpp_5707053959233246
