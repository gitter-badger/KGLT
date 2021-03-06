#include <Rocket/Core/Element.h>
#include <Rocket/Core/ElementText.h>
#include <Rocket/Core/ElementDocument.h>

#include "interface.h"
#include "ui_private.h"
#include "../window_base.h"

namespace kglt {
namespace ui {

const std::string HIDDEN = "1";
const std::string VISIBLE = "0";

Element::Element(std::shared_ptr<ElementImpl> impl):
    impl_(impl) {

}

Element Element::append(const unicode &tag) {
    return impl_->append(tag);
}

void Element::text(const unicode& text) {
    impl_->set_text(text);
}

const unicode Element::text() const {
    return impl_->text();
}

void Element::remove_class(const unicode &cl) {
    for(auto cls: cl.split(" ")) {
        impl_->remove_class(cls.encode());
    }
}

void Element::add_class(const unicode &cl) {
    for(auto cls: cl.split(" ")) {
        impl_->add_class(cls.encode());
    }
}

void Element::remove_children() {
    impl_->remove_children();
}

void Element::inner_rml(const unicode& rml) {
    impl_->inner_rml(rml);
}

std::string Element::css(const std::string& property) const {
    return impl_->css(property);
}

void Element::css(const std::string& property, const std::string& value) {
    impl_->css(property, value);
}

void Element::attr(const std::string& property, const std::string& value) {
    impl_->attr(property, value);
}

void Element::id(const std::string& id) {
    impl_->id(id);
}

void Element::scroll_to_bottom() {
    impl_->scroll_to_bottom();
}

void Element::show(const std::string& transition) {
    if(transition == "fade") {
        css("opacity", "0"); // Make transparent

        // Wrap a new refrence to the impl
        Element copy(impl_);

        // Trigger a task to increase the opacity in the background
        idle().add([copy]() mutable -> bool {
            auto current = unicode(copy.css("opacity")).to_float();
            current += 0.01;
            if(current >= 1.0) {
                copy.css("opacity", "1");
                // Make visible
                return false;
            }
            copy.css("opacity", std::to_string(current));
            return true;
        });
    }
    // Make visible
    css("visibility", "visible");
}

IdleTaskManager& Element::idle() {
    // Sigh...
    auto* window = this->impl_->_rocket_impl()->interface()->window();
    return window->idle;
}

bool Element::is_visible() const {
    auto visibility = css("visibility");
    return visibility != HIDDEN;
}

void Element::set_event_callback(const unicode& event_type, std::function<bool (Event)> func) {
    impl_->set_event_callback(event_type, func);
}

float Element::left() const {
    return impl_->left();
}

float Element::top() const {
    return impl_->top();
}

float Element::width() const {
    return impl_->width();
}

float Element::height() const {
    return impl_->height();
}

}

}
